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
#include <stack>

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
PCDSimulator<PriorityComp>::PCDSimulator(const std::vector<Task>& tasks, std::size_t num_procs, std::size_t num_queues) : 
	_priority(), _schedulable(true), _num_procs(num_procs), _num_queues(num_queues), 
	_running_job(num_procs), _idle(num_procs, true), _tasks(tasks), _enabled_tasks(tasks.size(), true),
	_enabled_procs(num_procs, true), _ready_jobs(num_queues), _current_jobs(), _completed_jobs(), 
	_t_reached(0), _executions(num_procs), _preemptions(num_procs) {

	}

template<typename PriorityComp>
std::size_t PCDSimulator<PriorityComp>::id2pos(unsigned task_id) const {
	return (std::size_t) (std::find_if(_tasks.begin(), _tasks.end(), [task_id](const Task& task) { return task_id == task.id; }) - _tasks.begin());
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::execute_job(unsigned t, std::size_t p){
	if(! _idle[p]) {
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
void PCDSimulator<PriorityComp>::terminate_running_job(std::size_t p){
	_completed_jobs.push_back(_running_job[p]);
	_current_jobs.erase(std::remove_if(_current_jobs.begin(), _current_jobs.end(), 
		[&](const Job& job){ return job.task_id == _running_job[p].task_id; }), _current_jobs.end());
	_idle[p] = true;
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::incoming_jobs(unsigned t) {
	for(const Task& task : _tasks){
		if(task_enabled(task) && (((int)t - (int)task.o) % (int)task.t == 0)){
			Job job = Job(task, t);
			add_job(job, job_queue(job.task_id));
		}
	}
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::add_job(const Job& job, std::size_t q){
        _current_jobs.push_back(job);
        _ready_jobs[q].push(job);
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::preempt(std::size_t p, std::size_t q){
	_ready_jobs[q].push(_running_job[p]);
	_running_job[p] = _ready_jobs[q].top();
	_ready_jobs[q].pop();
	++_preemptions[p];
}

template<typename PriorityComp>
bool PCDSimulator<PriorityComp>::check_deadlines(unsigned t){
	return (std::any_of(_current_jobs.begin(), _current_jobs.end(), 
		[t, this](const Job& job){ 
			if(job.missed(t)){
				_fail_t = t;
				_fail_id = job.task_id;
				return true;
			}
			return false;
		})) ? DEADLINES_NOT_OK : DEADLINES_OK;
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::disable_task(const Task& task, bool remove_job) { 
	_enabled_tasks[id2pos(task.id)] = false;
	if(remove_job){
		// Erase task job in _current_jobs vector
		_current_jobs.erase(std::remove_if(_current_jobs.begin(), _current_jobs.end(), 
			[task](const Job& job){ return job.task_id == task.id; }), _current_jobs.end());

		// If the task job is currently assigned to a processor, set said processor to idle state
		for(std::size_t i = 0; i < _running_job.size(); ++i){
			if(_running_job[i].task_id == task.id){ _idle[i] = true; }
		}

		// Finally, remove the task job from its _read_jobs priority queue
		std::size_t q = job_queue(task.id);
		std::stack<Job> other_jobs;
		bool found = false;
		while((! found) && (! _ready_jobs[q].empty())){
			if(_ready_jobs[q].top().task_id == task.id){
				found = true;
			}
			else{
				other_jobs.push(_ready_jobs[q].top());
			}
			_ready_jobs[q].pop();
		}
		while(! other_jobs.empty()){
			_ready_jobs[q].push(other_jobs.top());
			other_jobs.pop();
		}
	}
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::disable_proc(std::size_t p) {
	_enabled_procs[p] = false;
	if(! _idle[p]){
		_ready_jobs[job_queue(_running_job[p].task_id)].push(_running_job[p]);
	}
	_idle[p] = true;
}

template<typename PriorityComp>
bool PCDSimulator<PriorityComp>::task_enabled(const Task& task) const { 
	return _enabled_tasks[id2pos(task.id)];
}

template<typename PriorityComp>
bool PCDSimulator<PriorityComp>::proc_enabled(std::size_t p) const {
	return _enabled_procs[p];
}

template<typename PriorityComp>
bool PCDSimulator<PriorityComp>::run(){
	unsigned t = 0;
	while(std::any_of(_enabled_procs.begin(), _enabled_procs.end(), [](bool enabled){ return enabled; })){
		incoming_jobs(t);
		schedule();
		for(std::size_t p = 0; p < _num_procs; ++p){ if(proc_enabled(p)){ execute_job(t, p); }}
		if(check_deadlines(t) == DEADLINES_NOT_OK){
			_t_reached = t;
			_schedulable = false;
			break;
		}
		time_step(t);
		++t;
	}
	return _schedulable;
}

template<typename PriorityComp>
unsigned PCDSimulator<PriorityComp>::time_enabled(std::size_t p) const{
	return (unsigned) (_executions[p].size() - 1);
}

template<typename PriorityComp>
unsigned PCDSimulator<PriorityComp>::procs_used() const {
	return (unsigned) (std::find_if(_executions.begin(), _executions.end(), 
		[](const std::vector<int>& proc_execs){
			return std::all_of(proc_execs.begin(), proc_execs.end(), [](int exec){ return exec == IDLE_EXEC; });
		}) - _executions.begin());
}

template<typename PriorityComp>
std::vector<unsigned> PCDSimulator<PriorityComp>::idle_time() const{
	std::vector<unsigned> idle(_num_procs);
	for(std::size_t i = 0; i < idle.size(); ++i){
		idle[i] = (unsigned) std::count(_executions[i].begin(), _executions[i].end(), IDLE_EXEC);
	}
	return idle;
}

template<typename PriorityComp>
unsigned PCDSimulator<PriorityComp>::tot_idle_time() const{
	std::vector<unsigned> idle = idle_time();
	return (unsigned) std::accumulate(idle.begin(), idle.end(), 0);
}

template<typename PriorityComp>
std::vector<unsigned> PCDSimulator<PriorityComp>::preemptions() const{
	return _preemptions;
}

template<typename PriorityComp>
unsigned PCDSimulator<PriorityComp>::tot_preemptions() const{
	return (unsigned) std::accumulate(_preemptions.begin(), _preemptions.end(), 0);
}

template<typename PriorityComp>
std::vector<double> PCDSimulator<PriorityComp>::utilization() const{
	std::vector<unsigned> idle = idle_time();
	std::vector<double> utils(_num_procs);
	for(std::size_t i = 0; i < utils.size(); ++i){
		utils[i] = 1.0 - (idle[i] / ((double)_executions[i].size()));
	}
	return utils;
}

template<typename PriorityComp>
double PCDSimulator<PriorityComp>::tot_utilization() const{
	return 1.0 - (tot_idle_time() / std::accumulate(_executions.begin(), _executions.end(), 0.0,
		[](const double& sum, const std::vector<int>& proc_execs){
			return sum + (double) proc_execs.size();
		}));
}

template<typename PriorityComp>
std::string PCDSimulator<PriorityComp>::stringify_simulation() {
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
std::string PCDSimulator<PriorityComp>::stringify_missed_deadline() {
	std::stringstream ss;
	ss 	<< "Missed deadline for job of task " << _fail_id << " at time " 
		<< _fail_t << ".";
	return ss.str();
}

template<typename PriorityComp>
void PCDSimulator<PriorityComp>::prettify_simulation(const std::string& filename) {
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
unsigned PCDSimulator<PriorityComp>::hyper_period(const std::vector<Task>& tasks) {
	return (unsigned)std::accumulate(tasks.begin(), tasks.end(), 0, [](const unsigned& sum, const Task& task){ return sum + task.t; });
}

template<typename PriorityComp>
unsigned PCDSimulator<PriorityComp>::feasibility_interval(const std::vector<Task>& tasks) {
	if(tasks.empty()) { return 0; }
	return (*std::max_element(tasks.begin(), tasks.end(), [](const Task& x, const Task& y){ return x.o < y.o; })).o + 2 * hyper_period(tasks);
}

bool DMPriority::operator() (const Job& a, const Job& b) const {
	return (a.d_rel == b.d_rel) ? a.task_id > b.task_id : a.d_rel > b.d_rel;
}

PDMSimulator::PDMSimulator(const std::vector<Task>& tasks, unsigned partitions) : 
	PCDSimulator<DMPriority>(tasks, partitions, partitions), 
	_partitioning(partitions), _task_partition(), _partitionable(true), 
	_feasibility_intervals(partitions) {
		partition_tasks(partitions);
		if(_partitionable){
			for(unsigned partition = 0; partition < partitions; ++partition){
				_feasibility_intervals[partition] = PCDSimulator::feasibility_interval(_partitioning[partition]);
			}
		}
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
	  		_partitionable = false;
	  		break;
		}
	}
}

std::size_t PDMSimulator::job_queue(unsigned task_id){
	return (std::size_t)_task_partition[task_id];
}

void PDMSimulator::schedule(){
	for(std::size_t p = 0; p < _num_procs; ++p){
		if(proc_enabled(p) && ! _ready_jobs[p].empty()){
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

void PDMSimulator::time_step(unsigned t){
	for(std::size_t i = 0; i < _partitioning.size(); ++i){
		if(t == _feasibility_intervals[i]){
			disable_proc(i);
			for(const Task& task : _partitioning[i]){
				disable_task(task, true);
			}
		}
	}
}

bool PDMSimulator::run(){
	if(_partitionable){
		return PCDSimulator::run();
	}
	return false;
}

std::string PDMSimulator::stringify_partitions() {
	std::stringstream ss;
	unsigned p = 1;
	for(const std::vector<Task>& tasks : _partitioning){
		double acc_u = 0.0;
		ss << "Partition " << p++ << " : ";
		for(const Task& task : tasks){
			ss << task << " / ";
			acc_u += task.u;
		}
		ss << "u = " << acc_u << std::endl;
	}
	return ss.str();
}


unsigned PDMSimulator::partitions_used() const {
	return (unsigned) (std::find_if(_partitioning.begin(), _partitioning.end(), 
		[](const std::vector<Task>& partition){ return partition.empty(); }) - _partitioning.begin());
}


GDMSimulator::GDMSimulator(const std::vector<Task>& tasks, unsigned procs) : 
	PCDSimulator<DMPriority>(tasks, procs, 1), 
	_feasibility_interval(PCDSimulator::feasibility_interval(tasks)) {

	}

std::size_t GDMSimulator::job_queue(unsigned task_id) {
	return SHARED_QUEUE;
}

void GDMSimulator::schedule(){
	for(std::size_t p = 0; p < _num_procs; ++p){
		if(proc_enabled(p) && _idle[p] && ! _ready_jobs[SHARED_QUEUE].empty()){
			_running_job[p] = _ready_jobs[SHARED_QUEUE].top();
			_ready_jobs[SHARED_QUEUE].pop();
			_idle[p] = false;
		}	
	}
	if(! _ready_jobs[SHARED_QUEUE].empty()){
		bool unassigned_higher_priority;
		do{
			unassigned_higher_priority = false;
			std::size_t lowest_priority_proc = (std::size_t) (std::find(_enabled_procs.begin(), _enabled_procs.end(), true) - _enabled_procs.begin());
			for(std::size_t p = lowest_priority_proc + 1; p < _num_procs; ++p){
				if(proc_enabled(p) && _priority(_running_job[p], _running_job[lowest_priority_proc])){
					lowest_priority_proc = p;
				}
			}
			if(_priority(_running_job[lowest_priority_proc], _ready_jobs[SHARED_QUEUE].top())){
				unassigned_higher_priority = true;
				preempt(lowest_priority_proc, SHARED_QUEUE);
			}
		} while(unassigned_higher_priority && ! _ready_jobs[SHARED_QUEUE].empty());	
	}
}

void GDMSimulator::time_step(unsigned t){
	if(t == _feasibility_interval){
		for(std::size_t i = 0; i < _num_procs; ++i){
			disable_proc(i);
		}
	}
}












