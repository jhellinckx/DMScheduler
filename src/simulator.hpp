#ifndef __SIMULATOR_HPP
#define __SIMULATOR_HPP

#include <vector>
#include "task.hpp"
#include "job.hpp"

class Simulator{
protected:
	std::vector<Task> _tasks;
	std::vector<Job> _jobs;
	std::vector<Job> _completed;

public:
	Simulator(const std::vector<Task>& tasks) : _tasks(tasks), _jobs() {}

	void run(unsigned t_max){
		for(unsigned t = 0; t < t_max; ++t){
			generate_jobs(t);
			if(_jobs.size() > 0){
				schedule(next_job(), t);
			}
			if(check_deadlines() == false){
				std::cout << "not schedulable" << std::endl;
				break;
			}
		}
	}

	void generate_jobs(unsigned t) {
		for(const Task& task : _tasks){
			if(((int)t - (int)task.o) % (int)task.t == 0){
				_jobs.push_back(Job(task, t)):
			}
		}
	}

	void schedule(std::size_t job, unsigned t){
		_jobs[job].execute(t);
		if(_jobs[job].completed()){
			_completed.push_back(_jobs[job]);
			_jobs.erase(_jobs.begin() + job);
		}
	}

	bool check_deadlines(){
		bool respected = true;
		for(const Job& job : _jobs){
			if(job.missed()){
				respected = false;
			}
		}
		return respected;
	}

	virtual std::size_t next_job() = 0;

};


class DMSimulator : public Simulator{

public:
	DMSimulator(const std::vector<Task>& tasks) : Simulator(tasks) {}

	std::size_t next_job() {
		return std::min_element(_jobs.begin(), _jobs.end(), [](const Job& first, Job& second){
			return first.d_rel < second.d_rel;
		}) - _jobs.begin();
	}
};

#endif