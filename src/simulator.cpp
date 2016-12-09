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
#define SHARED_QUEUE 0


template<typename PriorityComp>
void FTPSimulator<PriorityComp>::set_tasks_id() {
	for(std::size_t i = 0; i < _tasks.size(); ++i){ _tasks[i].id = (unsigned) i + 1; }
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::execute_job(unsigned t, std::size_t p){
	if(! _idle[p]){
		_running_job[p].execute(t);
		_executions[p].push_back((int)_running_job[p].task_id);
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
			Job job = Job(task, t);
			add_job(job, (std::size_t)job_queue(job));
		}
	}
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::add_job(const Job& job, std::size_t q){
        _current_jobs.push_back(job);
        _ready_jobs[q].push(job);
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
	for(unsigned t = 0; t <= feasibility_interval(); ++t){
		incoming_jobs(t);
		schedule();
		for(std::size_t p = 0; p < _num_procs; ++p){ execute_job(t, p); }
		if(check_deadlines(t) == DEADLINES_NOT_OK){
			_t_reached = t;
			_schedulable = false;
			break;
		}
	}
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
	for(std::size_t p = 0; p < _num_procs; ++p){
		ss << "Processor " << p + 1 << std::endl;
		ss << "Job running : ";
		if(_idle[p]){ ss << "None"; } else { ss <<  _running_job[p]; }
		ss 	<< std::endl
			<< "Processor executions : " << std::endl
			<< _executions[p] << std::endl;	
	}
	for(std::size_t q = 0; q < _num_queues; ++q){
		ss << "Jobs ready for queue " << q + 1 << " : " << std::endl;
		while(!_ready_jobs[q].empty()){
			ss << _ready_jobs[q].top() << std::endl;
			_ready_jobs[q].pop();
		}	
	}
	
	ss << "Completed Jobs : " << std::endl;
	for(const Job& job : _completed_jobs){
		ss << job << std::endl;
	}
	return ss.str();
}

template<typename PriorityComp>
void FTPSimulator<PriorityComp>::prettify_simulation(const std::string& filename) {
	std::stringstream ss;
	ss << "python " << PYTHON_PRETTIFIER_FILENAME << " " << filename << " ";
	ss << "\"[";
	for(std::size_t i = 0; i < _executions.size(); ++i){
		ss << "[";
		for(std::size_t j = 0; j < _executions[i].size(); ++j){
			ss << _executions[i][j];
			if(j < _executions[i].size() - 1){ ss << ","; }
		}
		ss << "]";
		if(i < _executions.size() - 1){ ss << ","; }
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

template<typename PriorityComp>
unsigned FTPSimulator<PriorityComp>::hyper_period(const std::vector<Task>& tasks) const {
	return (unsigned)std::accumulate(tasks.begin(), tasks.end(), 0, [](const unsigned& sum, const Task& task){ return sum + task.t; });
}

template<typename PriorityComp>
unsigned FTPSimulator<PriorityComp>::feasibility_interval(const std::vector<Task>& tasks) const {
	return (*std::max_element(tasks.begin(), tasks.end(), [](const Task& x, const Task& y){ return x.o < y.o; })).o + 2 * hyper_period(tasks);
}

template<typename PriorityComp>
unsigned FTPSimulator<PriorityComp>::feasibility_interval() const { return feasibility_interval(_tasks); }


bool DMPriority::operator() (const Job& a, const Job& b) const {
	return (a.d_rel == b.d_rel) ? a.task_id > b.task_id : a.d_rel > b.d_rel;
}

PDMSimulator::PDMSimulator(const std::vector<Task>& tasks, unsigned partitions) : 
	FTPSimulator<DMPriority>(tasks, partitions, partitions), 
	_partitioning(partitions), _task_partition() {
		partition_tasks(partitions);
	}

void PDMSimulator::partition_tasks(unsigned partitions){
	// Sort by decreasing utilization
	std::sort(_tasks.begin(), _tasks.end(),
            [](Task x, Task y) { return x.u >= y.u; });
	std::vector<double> utilization(partitions, 0.0);
	for (const Task &task : _tasks) {
		unsigned partition = 0;
		double min_diff = 1.0;
		bool placed = false;
		for (unsigned i = 0; i < partitions; ++i) {
			double utilization_left = fabs(task.u - (1.0 - utilization[i]));
			if (utilization[i] + task.u <= 1.0 && utilization_left < min_diff) {
				min_diff = utilization_left;
				placed = true;
				partition = i;
			}
		}
		if (placed) {
			_partitioning[partition].push_back(task);
			_task_partition[task.id] = partition;
	  		utilization[partition] += task.u;
		} else {
	  		std::cout << "No Partition here" << std::endl;
	  		break;
		}
	}
}

unsigned PDMSimulator::job_queue(const Job& job){
	return _task_partition[job.task_id];
}

void PDMSimulator::schedule(){
	for(std::size_t p = 0; p < _num_procs; ++p){
		if(! _ready_jobs[p].empty()){
			if(_idle[p]){
				_running_job[p] = _ready_jobs[p].top();
				_ready_jobs[p].pop();
				_idle[p] = false;
			}
			else if(_priority(_running_job[p], _ready_jobs[p].top())){
				preempt(p, p);
			}
		}	
	}
}

unsigned PDMSimulator::feasibility_interval() const {
	std::vector<unsigned> intervals(_partitioning.size());
	std::transform(_partitioning.begin(), _partitioning.end(), intervals.begin(), 
		[this](const std::vector<Task>& tasks){ return FTPSimulator::feasibility_interval(tasks); });
	return *(std::max_element(intervals.begin(), intervals.end()));
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


GDMSimulator::GDMSimulator(const std::vector<Task>& tasks, unsigned procs) : 
	FTPSimulator<DMPriority>(tasks, procs, 1) {

	}

unsigned GDMSimulator::job_queue(const Job& job) {
	return SHARED_QUEUE;
}

void GDMSimulator::schedule(){
	for(std::size_t p = 0; p < _num_procs; ++p){
		if(_idle[p] && ! _ready_jobs[SHARED_QUEUE].empty()){
			_running_job[p] = _ready_jobs[SHARED_QUEUE].top();
			_ready_jobs[SHARED_QUEUE].pop();
			_idle[p] = false;
		}	
	}
	if(! _ready_jobs[SHARED_QUEUE].empty()){
		bool unassigned_higher_priority;
		do{
			unassigned_higher_priority = false;
			std::size_t lowest_priority_proc = (std::size_t) (std::min_element(_running_job.begin(), _running_job.end(), _priority) - _running_job.begin());
			if(_priority(_running_job[lowest_priority_proc], _ready_jobs[SHARED_QUEUE].top())){
				unassigned_higher_priority = true;
				preempt(lowest_priority_proc, SHARED_QUEUE);
			}
		} while(unassigned_higher_priority && ! _ready_jobs[SHARED_QUEUE].empty());	
	}
}












