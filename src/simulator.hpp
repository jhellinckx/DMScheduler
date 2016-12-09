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
#include <map>

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

	FTPSimulator(const std::vector<Task>& tasks, std::size_t p, std::size_t q);
	void set_tasks_id();
	void execute_job(unsigned t, std::size_t p);
	void terminate_running_job(std::size_t p);
	void incoming_jobs(unsigned t);
	void add_job(const Job& job, std::size_t q);
	void preempt(std::size_t p, std::size_t q);
	bool check_deadlines(unsigned t);

	virtual unsigned job_queue(const Job& job) = 0;
	virtual void schedule() = 0;

public:
	virtual void run();
	virtual unsigned hyper_period(const std::vector<Task>& tasks) const;
	virtual unsigned feasibility_interval(const std::vector<Task>& tasks) const;
	virtual unsigned feasibility_interval() const;
	virtual std::string stringify_simulation();
	virtual void prettify_simulation(const std::string& filename);

	virtual ~FTPSimulator(){}
};

class DMPriority{
public:
	bool operator() (const Job& a, const Job& b) const;
};

class PDMSimulator : public FTPSimulator<DMPriority>{
	std::vector<std::vector<Task>> _partitioning;
	std::map<unsigned, unsigned> _task_partition;

	void partition_tasks(unsigned partitions);

protected:
	unsigned job_queue(const Job& job);
	void schedule();
	
public:
	PDMSimulator(const std::vector<Task>& tasks, unsigned partitions);
	unsigned feasibility_interval() const;
	std::string stringify_partitions();
	virtual ~PDMSimulator() {}
};

class GDMSimulator : public FTPSimulator<DMPriority>{
	
	virtual ~GDMSimulator() {}

};

#endif