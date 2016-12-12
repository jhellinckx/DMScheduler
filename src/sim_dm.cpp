#include <iostream>
#include <cstdlib>
#include <string>
#include <vector>
#include <fstream>
#include <cstdio>
#include <iostream>
#include <utility>
#include <cmath>

#include "task.hpp"
#include "simulator.hpp"
#include "utils.hpp"

const unsigned EXPECTED_ARGC = 4;
const unsigned STRATEGY_ARGPOS = 1;
const unsigned FILENAME_ARGPOS = 2;
const unsigned NUM_PROCS_ARGPOS = 3;

const std::string PARTITION_OPTION = "-p";

const unsigned EXP_BASE = 2;

std::vector<Task> read_tasks(std::ifstream in) {
  std::string task_string;
  std::vector<Task> tasks;
  Task task;
  unsigned id = 1;
  while (in.peek() != EOF) {
    in >> task.o >> task.t >> task.d >> task.c;
	in.get(); // Consume \n
    task.u = ((double)task.c) / task.t;
    task.id = id++;
    tasks.push_back(task);
  }
  return tasks;
}

unsigned min_procs_partition(const std::vector<Task>& tasks, unsigned base){
	unsigned partitions = (base == 0) ? 1 : base * EXP_BASE;
	PDMSimulator sim{tasks, base};
	while(! (sim = PDMSimulator(tasks, partitions)).partitionable()){
		partitions *= EXP_BASE;
	}
	return sim.partitions_used();
}

unsigned bs_min_procs_global(const std::vector<Task>& tasks, const std::pair<unsigned, unsigned>& range){
	if(range.first == range.second) { return range.first; }
	unsigned mid = ((unsigned) std::ceil((range.second - range.first) / 2.0)) + range.first;
	if(GDMSimulator(tasks, mid).run()){
		return bs_min_procs_global(tasks, std::make_pair(range.first, mid));
	}
	else{
		return bs_min_procs_global(tasks, std::make_pair(mid + 1, range.second));
	}
}

unsigned min_procs_global(const std::vector<Task>& tasks, const unsigned base, bool schedulable){
	unsigned schedulable_procs = base;
	if(! schedulable){
		schedulable_procs = (base == 0) ? 1 : base * EXP_BASE;
		while(! GDMSimulator(tasks, schedulable_procs).run()){
			schedulable_procs *= EXP_BASE;
		}
	}
	return bs_min_procs_global(tasks, std::make_pair(0, schedulable_procs));
}

void print_infos(const PCDSimulator<DMPriority>& sim, unsigned num_procs){
	std::vector<unsigned> idle(sim.idle_time());
	std::vector<unsigned> preempts(sim.preemptions());
	std::vector<double> utils(sim.utilization());
	for(std::size_t p = 0; p < num_procs; ++p){
		std::cout << std::string(20, '-') << std::endl;
		std::cout << "Processor " << p + 1 << " : " << std::endl;
		std::cout << "Interval " << sim.time_enabled(p) << std::endl;
		std::cout << "Idle time " << idle[p] << std::endl;
		std::cout << "Preemptions " << preempts[p] << std::endl;
		std::cout << "Utilization " << utils[p] << std::endl;
	}
	std::cout << std::string(20, '-') << std::endl;
	std::cout << "Totals : " << std::endl;
	std::cout << "Idle time " << sim.tot_idle_time() << std::endl;
	std::cout << "Preemptions " << sim.tot_preemptions() << std::endl;
	std::cout << "Utilization " << sim.tot_utilization() << std::endl;
}

int main(int argc, char** argv){
	if(argc != EXPECTED_ARGC){
		std::cout << "Invalid number of arguments. Expected " << EXPECTED_ARGC << ", got " << argc << "." << std::endl;
		return EXIT_FAILURE;
	}
	bool partition = (argv[STRATEGY_ARGPOS] == PARTITION_OPTION);
	std::string tasks_file = argv[FILENAME_ARGPOS];
	unsigned num_procs = (unsigned) atoi(argv[NUM_PROCS_ARGPOS]);
	
	std::vector<Task> tasks = read_tasks(std::ifstream(tasks_file));

	if(partition){
		PDMSimulator sim(tasks, num_procs);
		sim.run();
		sim.prettify_simulation("schedule.png");
		if(! sim.partitionable()){
			std::cout << "Cannot partition the given tasks set into " << num_procs << " processor(s) only." << std::endl
			<< "Try with " << min_procs_partition(tasks, num_procs) << " processor(s) instead." << std::endl;
		}
		else{
			std::cout << sim.stringify_partitions();
			if(sim.schedulable()){
				std::cout << "Successfully scheduled the given system using the partitioned strategy (best fit) with " << num_procs << " processors." << std::endl;
				std::cout << "Processors used : " << sim.partitions_used() << std::endl;
				print_infos(sim, num_procs);
			}
			else{
				std::cout << "The given system could not be scheduled using the partitioned strategy (best fit) with " << num_procs << " processors." << std::endl;
				std::cout << "Reason : " << sim.stringify_missed_deadline() << std::endl;
				std::cout << "The best fit heuristic we use will always give us identical partitions even if we increase the number of processors." << std::endl
				<< "Hence, we simply cannot schedule the given tasks set using the partition strategy with best fit." << std::endl;
			}
		}
	}
	else{
		GDMSimulator sim(tasks, num_procs);
		sim.run();
		sim.prettify_simulation("schedule.png");
		if(sim.schedulable()){
			std::cout << "Successfully scheduled the given system using the global strategy with " << num_procs << " processors." << std::endl;
			std::cout << "Processors used : " << sim.procs_used() << std::endl;
			std::cout << "Minimum processors needed : " << min_procs_global(tasks, num_procs, true) << std::endl;
			print_infos(sim, num_procs);
		}
		else{
			std::cout << "The given system could not be scheduled using the global strategy with " << num_procs << " processors." << std::endl;
			std::cout << "Reason : " << sim.stringify_missed_deadline() << std::endl;
			std::cout << "Minimum processors needed : " << min_procs_global(tasks, num_procs, false) << std::endl;
		}
	}

	return EXIT_SUCCESS;
}