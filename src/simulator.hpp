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
class PCDSimulator{
private:
	std::size_t id2pos(unsigned task_id) const;

protected:
	PriorityComp _priority;
	bool _schedulable;
	std::size_t _num_procs;
	std::size_t _num_queues;
	std::vector<Job> _running_job;
	std::vector<bool> _idle;
	std::vector<Task> _tasks;
	std::vector<bool> _enabled_tasks;
	std::vector<bool> _enabled_procs;
	std::vector<std::priority_queue<Job, std::vector<Job>, PriorityComp>> _ready_jobs;
	std::vector<Job> _current_jobs;
	std::vector<Job> _completed_jobs;
	unsigned _t_reached;
	std::vector<std::vector<int>> _executions;
	std::vector<unsigned> _preemptions;

	PCDSimulator(const std::vector<Task>& tasks, std::size_t p, std::size_t q);
	void execute_job(unsigned t, std::size_t p);
	void terminate_running_job(std::size_t p);
	void incoming_jobs(unsigned t);
	void add_job(const Job& job, std::size_t q);
	void preempt(std::size_t p, std::size_t q);
	bool check_deadlines(unsigned t);

	void disable_task(const Task& task, bool remove_job = false);
	void disable_proc(std::size_t p);
	bool task_enabled(const Task& task) const;
	bool proc_enabled(std::size_t p) const;

	virtual void time_step(unsigned t) = 0;
	virtual std::size_t job_queue(unsigned task_id) = 0;
	virtual void schedule() = 0;

public:
	static unsigned hyper_period(const std::vector<Task>& tasks);
	static unsigned feasibility_interval(const std::vector<Task>& tasks);

	virtual bool run();
	virtual bool schedulable() const { return _schedulable; }
	virtual std::string stringify_simulation();
	virtual void prettify_simulation(const std::string& filename);
	virtual unsigned procs_used() const;
	virtual std::vector<unsigned> idle_time() const;
	virtual unsigned tot_idle_time() const;
	virtual std::vector<unsigned> preemptions() const;
	virtual unsigned tot_preemptions() const;
	virtual std::vector<double> utilization() const;
	virtual double tot_utilization() const;
	virtual ~PCDSimulator(){}
};



class DMPriority{
public:
	bool operator() (const Job& a, const Job& b) const;
};

class PDMSimulator : public PCDSimulator<DMPriority>{
	std::vector<std::vector<Task>> _partitioning;
	std::map<unsigned, unsigned> _task_partition;
	bool _partitionable;
	std::vector<unsigned> _feasibility_intervals;

	void partition_tasks(unsigned partitions);

protected:
	void time_step(unsigned t);
	std::size_t job_queue(unsigned task_id);
	void schedule();
	
public:
	PDMSimulator(const std::vector<Task>& tasks, unsigned partitions);
	std::string stringify_partitions();

	unsigned partitions_used() const;

	bool partitionable() const { return _partitionable; }
	virtual bool run();
	virtual ~PDMSimulator() {}
};



class GDMSimulator : public PCDSimulator<DMPriority>{
	unsigned _feasibility_interval;
protected:
	void time_step(unsigned t);
	std::size_t job_queue(unsigned task_id);
	void schedule();

public:
	GDMSimulator(const std::vector<Task>& tasks, unsigned procs);

	virtual ~GDMSimulator() {}

};

#endif