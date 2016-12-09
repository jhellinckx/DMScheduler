#ifndef __SIMULATOR_HPP
#define __SIMULATOR_HPP

#include <vector>
#include "task.hpp"
#include "job.hpp"
#include <algorithm>
#include <queue>
#include <string>
#include <sstream>
#include <numeric>

template<typename PriorityComp>
class FTPSimulator{
protected:
	PriorityComp _priority;
	bool _schedulable;
	std::size_t _num_procs;
	std::size_t _num_queues;
	std::vector<Job> _running_job;
	std::vector<bool> _idle;
	std::vector<Task> _tasks;
	std::vector<std::priority_queue<Job, std::vector<Job>, PriorityComp>> _ready_jobs;
	std::vector<Job> _current_jobs;
	std::vector<Job> _completed_jobs;
	unsigned _t_reached;
	std::vector<std::vector<int>> _executions;

	FTPSimulator(const std::vector<Task>& tasks);
	void set_tasks_id();
	void execute_job(unsigned t, std::size_t p);
	void terminate_running_job(std::size_t p);
	void incoming_jobs(unsigned t);
	void add_job(const Job& job, std::size_t q);
	void preempt(std::size_t p, std::size_t q);
	bool check_deadlines(unsigned t);

	virtual std::size_t job_queue(const Job& job) = 0;
	virtual void schedule() = 0;

public:
	virtual void run();
	virtual void clear();
	
	virtual std::string stringify_simulation();

	unsigned hyper_period() const {
		return (unsigned)std::accumulate(_tasks.begin(), _tasks.end(), 0, [](const unsigned& sum, const Task& task){ return sum + task.t; });
	}

	unsigned feasibility_interval() const {
		return (*std::max_element(_tasks.begin(), _tasks.end(), [](const Task& x, const Task& y){ return x.o < y.o; })).o + 2 * hyper_period();
	}

	std::vector<Task> tasks() const { return _tasks; }

	virtual ~FTPSimulator(){}
};

class DMPriority{
public:
	bool operator() (const Job& a, const Job& b) const;
};

class PDMSimulator : public FTPSimulator<DMPriority>{
	std::vector<std::vector<Task>> _partitioning;

	std::vector<bool> _schedulable_partitioning;
	std::vector<Job> _running_partitioning;
	std::vector<bool> _idle_partitioning;
	std::vector<std::priority_queue<Job, std::vector<Job>, DMPriority>> _ready_partitioning;
	std::vector<std::vector<Job>> _current_partitioning;
	std::vector<std::vector<Job>> _completed_partitioning;
	std::vector<unsigned> _t_reached_partitioning;
	std::vector<std::vector<int>> _executions_partitioning;
	

	void partition_tasks(unsigned partitions);

public:
	PDMSimulator(const std::vector<Task>& tasks, unsigned partitions);

	virtual void run();
	void save_partition(unsigned partition);
	void set_to_partition(unsigned partition);

	std::string stringify_partitions();
	std::string stringify_simulation();
	void prettify_simulation(const std::string& filename);

	virtual ~PDMSimulator() {}
};

class GDMSimulator : public FTPSimulator<DMPriority>{

	virtual ~GDMSimulator() {}

};

#endif