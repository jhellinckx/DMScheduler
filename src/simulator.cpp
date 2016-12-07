#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <cmath>
#include <string>

#include "task.hpp"
#include "job.hpp"
#include "simulator.hpp"

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::set_tasks_id() {
	for(std::size_t i = 0; i < _tasks.size(); ++i){ _tasks[i].id = (unsigned) i; }
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::execute_job(unsigned t){
	if(! _idle){
		_running_job.execute(t);
		if(_running_job.completed()){
			terminate_running_job();
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::add_job(const Job& job){
	_current_jobs.push_back(job);
	_ready_jobs.push(job);
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::terminate_running_job(){
	_completed_jobs.push_back(_running_job);
	_current_jobs.erase(std::remove_if(_current_jobs.begin(), _current_jobs.end(), 
		[&](const Job& job){ return job.task_id == _running_job.task_id; }), _current_jobs.end());
	_idle = true;
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::incoming_jobs(unsigned t) {
	for(const Task& task : _tasks){
		if(((int)t - (int)task.o) % (int)task.t == 0){
			std::cout << "Incoming job at time " << t << " : " << Job(task, t) << std::endl;
			add_job(Job(task, t));
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::schedule(){
	if(! _ready_jobs.empty()){
		if(_idle){
			_running_job = _ready_jobs.top();
			_ready_jobs.pop();
			_idle = false;
		}
		else if(_priority(_running_job, _ready_jobs.top())){
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
	_current_jobs(), _completed_jobs(), _t_reached(0) {
		set_tasks_id();
	}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::run(unsigned t_max){
	for(unsigned t = 0; t <= t_max; ++t){
		incoming_jobs(t);
		schedule();
		execute_job(t);
		if(check_deadlines(t) == DEADLINES_NOT_OK){
			_t_reached = t;
			break;
		}
	}
}


bool DMPriority::operator() (const Job& a, const Job& b) const {
	return (a.d_rel == b.d_rel) ? a.task_id > b.task_id : a.d_rel > b.d_rel;
}

PDMSimulator::PDMSimulator(const std::vector<Task>& tasks, unsigned partitions) : 
	FTPSimulator<DMPriority>(tasks), _partitioning(partitions) {
		partition_tasks(partitions);
	}

void PDMSimulator::partition_tasks(unsigned partitions){
	// Sort by decreasing utilization
	std::sort(_tasks.begin(), _tasks.end(),
            [](Task x, Task y) { return x.u >= y.u; });
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

std::string PDMSimulator::stringify_partitions() {
	std::stringstream ss;
	for(const std::vector<Task>& tasks : _partitioning){
		double acc_u = 0.0;
		ss << "Partition : ";
		for(const Task& task : tasks){
			ss << task << " / ";
			acc_u += task.u;
		}
		ss << "u = " << acc_u << std::endl;
	}
	return ss.str();
}

std::string PDMSimulator::stringify_simulation() {
	std::stringstream ss;
	if(!_current_jobs.empty()){
		ss 	<< "Scheduling failed at time " << _t_reached << "." << std::endl
			<< "Job running : ";
			if(_idle){ ss << "None"; } else { ss <<  _running_job; }
			ss 	<< std::endl
				<< "Jobs ready when deadline not respected : " << std::endl;
		while(!_ready_jobs.empty()){
			ss << _ready_jobs.top() << std::endl;
			_ready_jobs.pop();
		}		
	}
	else{
		ss << "Scheduling successful." << std::endl;
	}
	ss << "Completed Jobs : " << std::endl;
	for(const Job& job : _completed_jobs){
		ss << job << std::endl;
	}
	return ss.str();
}













