#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <cmath>

#include "task.hpp"
#include "job.hpp"
#include "simulator.hpp"

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::set_tasks_id() {
	for(std::size_t i = 0; i < _tasks.size(); ++i){ _tasks[i].id = i; }
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::execute_job(unsigned t){
	if(! _idle){
		_running_job.execute(t);
		if(_running_job.completed()){
		_completed_jobs.push_back(_running_job);
			_idle = true;
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::add_job(const Job& job){
	_current_jobs.push_back(job);
	_ready_jobs.push(job);
}
template<typename PriorityComp>
void FTPSimulator<PriorityComp>::incoming_jobs(unsigned t) {
	for(const Task& task : _tasks){
		if(((int)t - (int)task.o) % (int)task.t == 0){
			add_job(Job(task, t));
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::schedule(){
	if(! _ready_jobs.empty()){
		if(_idle || _priority(_ready_jobs.top(), _running_job)){
			preempt();			
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::preempt(){
	_ready_jobs.push(_running_job);
	_running_job = _ready_jobs.top();
	_ready_jobs.pop();
}

template<typename PriorityComp>
bool FTPSimulator<PriorityComp>::check_deadlines(unsigned t){
	return (std::any_of(_current_jobs.begin(), _current_jobs.end(), 
		[t](const Job& job){ return job.missed(t); })) ? DEADLINES_NOT_OK : DEADLINES_OK;
}

template<typename PriorityComp>
FTPSimulator<PriorityComp>::FTPSimulator(const std::vector<Task>& tasks) : 
	_priority(), _running_job(), _idle(true), _tasks(tasks), _ready_jobs(), 
	_current_jobs(), _completed_jobs() {
		set_tasks_id();
	}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::run(unsigned t_max){
	for(unsigned t = 0; t <= t_max; ++t){
		execute_job(t);
		incoming_jobs(t);
		schedule();
		if(check_deadlines(t) == DEADLINES_NOT_OK){
			std::cout << "not schedulable" << std::endl;
			break;
		}
	}
}


bool DMPriority::operator() (const Job& a, const Job& b) const {
	return (a.d == b.d) ? a.task_id < b.task_id : a.d < b.d;
}

PDMSimulator::PDMSimulator(const std::vector<Task>& tasks, unsigned partitions) : FTPSimulator<DMPriority>(tasks) {}

void PDMSimulator::partition_tasks(unsigned partitions){
	std::vector<double> utilization(partitions, 0.0);
	for (const Task &task : _tasks) {
		std::size_t partition = 0;
		double min_diff = 1.0;
		bool placed = false;
		for (std::size_t i = 0; i < partitions; ++i) {
			double utilization_left = fabs(task.u - (1.0 - utilization[i]));
			if (utilization[i] + task.u <= 1.0 && utilization_left < min_diff) {
				min_diff = utilization_left;
				placed = true;
				partition = i;
			}
		}
		if (placed) {
			_partitioning[partition].push_back(task);
	  		utilization[partition] += task.u;
		} else {
	  		std::cout << "No Partition here" << std::endl;
	  		break;
		}
	}
}














