#ifndef __SIMULATOR_HPP
#define __SIMULATOR_HPP

#define DEADLINES_OK true
#define DEADLINES_NOT_OK false

#include <vector>
#include "task.hpp"
#include "job.hpp"
#include <algorithm>
#include <queue>


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

	FTPSimulator(const std::vector<Task>& tasks);
	void set_tasks_id();
	void execute_job(unsigned t);
	void add_job(const Job& job);
	void incoming_jobs(unsigned t);
	void schedule();
	void preempt();
	bool check_deadlines(unsigned t);

public:
	virtual void run(unsigned t_max);

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

	virtual ~PDMSimulator() {}
};

class GDMSimulator : public FTPSimulator<DMPriority>{

	virtual ~GDMSimulator() {}

};

#endif