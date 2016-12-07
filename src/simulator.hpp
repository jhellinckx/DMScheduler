#ifndef __SIMULATOR_HPP
#define __SIMULATOR_HPP

#define DEADLINES_OK true
#define DEADLINES_NOT_OK false

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
	Job _running_job;
	bool _idle;
	std::vector<Task> _tasks;
	std::priority_queue<Job, std::vector<Job>, PriorityComp> _ready_jobs;
	std::vector<Job> _current_jobs;
	std::vector<Job> _completed_jobs;
	unsigned _t_reached;

	FTPSimulator(const std::vector<Task>& tasks);
	void set_tasks_id();
	void execute_job(unsigned t);
	void add_job(const Job& job);
	void terminate_running_job();
	void incoming_jobs(unsigned t);
	void schedule();
	void preempt();
	bool check_deadlines(unsigned t);

public:
	virtual void run(unsigned t_max);
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

	void partition_tasks(unsigned partitions);

public:
	PDMSimulator(const std::vector<Task>& tasks, unsigned partitions);

	std::string stringify_partitions();
	std::string stringify_simulation();

	virtual ~PDMSimulator() {}
};

class GDMSimulator : public FTPSimulator<DMPriority>{

	virtual ~GDMSimulator() {}

};

#endif