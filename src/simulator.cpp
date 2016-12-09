#include <vector>
#include <algorithm>
#include <queue>
#include <iostream>
#include <cmath>
#include <string>
#include <cstdlib>
#include <cctype>
#include <unistd.h>
#include <sstream>

#include "task.hpp"
#include "job.hpp"
#include "simulator.hpp"
#include "utils.hpp"

#define DEADLINES_OK true
#define DEADLINES_NOT_OK false
#define IDLE_EXEC -1
#define PYTHON_PRETTIFIER_FILENAME "prettify_schedule.py"

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::set_tasks_id() {
	for(std::size_t i = 0; i < _tasks.size(); ++i){ _tasks[i].id = (unsigned) i + 1; }
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::execute_job(unsigned t, std::size_t p){
	if(! _idle[p]){
		_running_job[p].execute(t);
		_executions[p].push_back((int)_running_job.task_id);
		if(_running_job[p].completed()){
			terminate_running_job(p);
		}
	}
	else{
		_executions[p].push_back(IDLE_EXEC);
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::terminate_running_job(std::size_t p){
	_completed_jobs.push_back(_running_job[p]);
	_current_jobs.erase(std::remove_if(_current_jobs.begin(), _current_jobs.end(), 
		[&](const Job& job){ return job.task_id == _running_job[p].task_id; }), _current_jobs.end());
	_idle[p] = true;
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::incoming_jobs(unsigned t) {
	for(const Task& task : _tasks){
		if(((int)t - (int)task.o) % (int)task.t == 0){
			Job job = Job(task, t):
			add_job(job, job_queue(job));
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::add_job(const Job& job, std::size_t q){
        _current_jobs.push_back(job);
        _ready_jobs[q].push(job);
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
void FTPSimulator<PriorityComp>::preempt(std::size_t p, std::size_t q){
	_ready_jobs[q].push(_running_job[p]);
	_running_job[p] = _ready_jobs[q].top();
	_ready_jobs[q].pop();
}

template<typename PriorityComp>
bool FTPSimulator<PriorityComp>::check_deadlines(unsigned t){
	return (std::any_of(_current_jobs.begin(), _current_jobs.end(), 
		[t](const Job& job){ return job.missed(t); })) ? DEADLINES_NOT_OK : DEADLINES_OK;
}

template<typename PriorityComp>
FTPSimulator<PriorityComp>::FTPSimulator(const std::vector<Task>& tasks, std::size_t num_procs, std::size_t num_queues) : 
	_priority(), _schedulable(true), _num_procs(num_procs), _num_queues(num_queues), 
	_running_job(num_procs), _idle(num_procs, true), _tasks(tasks), _ready_jobs(num_queues), 
	_current_jobs(), _completed_jobs(), _t_reached(0), _executions(num_procs) {
		set_tasks_id();
	}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::run(){
	for(unsigned t = 0; t < feasibility_interval(); ++t){
		incoming_jobs(t);
		schedule();
		for(std::size_t p = 0; p < num_procs; ++p){ execute_job(t, p); }
		if(check_deadlines(t) == DEADLINES_NOT_OK){
			_t_reached = t;
			_schedulable = false;
			break;
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::clear(){
	_schedulable = true;
	_running_job = Job();
	_idle = true;
	_tasks = std::vector<Task>();
	_ready_jobs = std::priority_queue<Job, std::vector<Job>, PriorityComp>();
	_current_jobs = std::vector<Job>();
	_completed_jobs = std::vector<Job>();
	_t_reached = 0;
	_executions = std::vector<int>();
}

template<typename PriorityComp>
std::string FTPSimulator<PriorityComp>::stringify_simulation() {
	std::stringstream ss;
	if(_schedulable){
		ss << "Scheduling successful." << std::endl;
	}
	else{
		ss 	<< "Scheduling failed at time " << _t_reached << "." << std::endl;
	}	
	ss << "Job running : ";
	if(_idle){ ss << "None"; } else { ss <<  _running_job; }
	ss 	<< std::endl
		<< "Jobs ready : " << std::endl;
	while(!_ready_jobs.empty()){
		ss << _ready_jobs.top() << std::endl;
		_ready_jobs.pop();
	}		
	ss << "Completed Jobs : " << std::endl;
	for(const Job& job : _completed_jobs){
		ss << job << std::endl;
	}
	ss << "Processor executions : " << std::endl;
	ss << _executions << std::endl;
	return ss.str();
}

bool DMPriority::operator() (const Job& a, const Job& b) const {
	return (a.d_rel == b.d_rel) ? a.task_id > b.task_id : a.d_rel > b.d_rel;
}

PDMSimulator::PDMSimulator(const std::vector<Task>& tasks, unsigned partitions) : 
	FTPSimulator<DMPriority>(tasks), _partitioning(partitions),
	_schedulable_partitioning(partitions), _running_partitioning(partitions), 
	_idle_partitioning(partitions), _ready_partitioning(partitions),
	_current_partitioning(partitions), _completed_partitioning(partitions),
	_t_reached_partitioning(partitions), _executions_partitioning(partitions) {
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

void PDMSimulator::run(){
	for(unsigned i = 0; i < (unsigned)_partitioning.size(); ++i){
		_tasks = _partitioning[i];
		FTPSimulator::run();
		save_partition(i);
		clear();
	}
}

void PDMSimulator::save_partition(unsigned partition){
	_running_partitioning[partition] = _running_job;
	_idle_partitioning[partition] = _idle;
	_ready_partitioning[partition] = _ready_jobs;
	_current_partitioning[partition] = _current_jobs;
	_completed_partitioning[partition] = _completed_jobs;
	_t_reached_partitioning[partition] = _t_reached;
	_executions_partitioning[partition] = _executions;
	_schedulable_partitioning[partition] = _schedulable;
}

void PDMSimulator::set_to_partition(unsigned partition){
	_tasks = _partitioning[partition];
	_schedulable = _schedulable_partitioning[partition];
	_running_job = _running_partitioning[partition];
	_idle = _idle_partitioning[partition];
	_ready_jobs = _ready_partitioning[partition];
	_current_jobs = _current_partitioning[partition];
	_completed_jobs = _completed_partitioning[partition];
	_t_reached = _t_reached_partitioning[partition];
	_executions = _executions_partitioning[partition];
}

std::string PDMSimulator::stringify_simulation() {
	std::stringstream ss;
	for(unsigned i = 0; i < (unsigned)_partitioning.size(); ++i){
		ss << "Simulation for partition " << i + 1 << " : " << std::endl;
		set_to_partition(i);
		ss << FTPSimulator::stringify_simulation();
		ss << std::endl; 
	}
	return ss.str();
}

void PDMSimulator::prettify_simulation(const std::string& filename){
	std::stringstream ss;
	ss << "python " << PYTHON_PRETTIFIER_FILENAME << " " << filename << " ";
	ss << "\"[";
	for(std::size_t i = 0; i < _executions_partitioning.size(); ++i){
		ss << "[";
		for(std::size_t j = 0; j < _executions_partitioning[i].size(); ++j){
			ss << _executions_partitioning[i][j];
			if(j < _executions_partitioning[i].size() - 1){ ss << ","; }
		}
		ss << "]";
		if(i < _executions_partitioning.size() - 1){ ss << ","; }
	}
	ss << "]\"";
	pid_t pid = fork();
    if(pid < 0){
        throw std::runtime_error("Failed to execute visual output command");
    }
    else if (pid == 0){
        if(system(NULL)){
            system(ss.str().c_str());
            _Exit(EXIT_SUCCESS);
        }
        else{
            _Exit(EXIT_FAILURE);
        }
    }
}














