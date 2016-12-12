

#include "generator.hpp"
#include "task.hpp"
#include "simulator.hpp"
#include "utils.hpp"

const std::size_t SAMPLE_SIZE_PER_VALUE = 10;

void prettify_plot(std::vector<double> xs, std::vector<double> ys){

}

void compare_load(){
	// Depending on the utilization with fixed and minimum proc numbers
	const int num_tasks = 8;
	const unsigned num_procs = 4;

	const double upper_u = 300;
	const double lower_u = 5;
	const double u_step = 5;

	std::vector<double> part_loads;
	std::vector<double> global_loads;
	std::vector<double> part_loads_min;
	std::vector<double> global_loads_min;
	std::vector<double> utils;
	std::vector<Task> tasks;

	for(double u = lower_u; u < upper_u; u += u_step){
		Generator gen(u, num_tasks);

		double tot_load_part = 0.0;
		double tot_load_global = 0.0;
		double schedulable_part = 0;
		double schedulable_global = 0;

		double tot_load_part_min = 0.0;
		double tot_load_global_min = 0.0;
		double schedulable_global_min = 0;
		double schedulable_part_min = 0;

		for(std::size_t i = 0; i < SAMPLE_SIZE_PER_VALUE; ++i){
			tasks = gen.create_tasks();
			// Fixed procs number
			PDMSimulator p_sim(tasks, num_procs);
			if(p_sim.run()){
				tot_load_part += p_sim.tot_utilization();
				++schedulable_part;
			}
			GDMSimulator g_sim(tasks, num_procs);
			if(g_sim.run()){
				tot_load_global += g_sim.tot_utilization();
				++schedulable_global;
			}
			// Min procs numbers
			PDMSimulator p_sim_min(tasks, PDMSimulator::min_partitions(tasks, num_procs));
			if(p_sim_min.run()){
				tot_load_part_min += p_sim_min.tot_utilization();
				++schedulable_part_min;
			}
			GDMSimulator g_sim_min(tasks, GDMSimulator::min_procs(tasks, num_procs));
			if(g_sim_min.run()){
				tot_load_global_min += g_sim_min.tot_utilization();
				++schedulable_global_min;
			}
		}

		part_loads.push_back(tot_load_part / schedulable_part);
		global_loads.push_back(tot_load_global / schedulable_global);
		part_loads_min.push_back(tot_load_part_min / schedulable_part_min);
		global_loads_min.push_back(tot_load_global_min / schedulable_global_min);
	}

	std::cout << part_loads << std::endl;
	std::cout << global_loads << std::endl;
	std::cout << part_loads_min << std::endl;
	std::cout << global_loads_min << std::endl;

	// Depending on the number of tasks
	const double utilization = 70;
	const unsigned lower_num_tasks = 1;
	const unsigned upper_num_tasks = 50;

}

void compare_required_procs(){

}

void compare_schedulability(){
	
}

int main(){
	compare_load();
	compare_required_procs();
	compare_schedulability();
}
