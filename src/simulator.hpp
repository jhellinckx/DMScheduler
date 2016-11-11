#ifndef __SIMULATOR_HPP
#define __SIMULATOR_HPP

#include <vector>
#include "task.hpp"
#include "job.hpp"

class Simulator{
protected:
	std::vector<Task> _tasks;
	std::vector<Job> _jobs;

public:
	Simulator(const std::vector<Task>& tasks) : _tasks(tasks), _jobs() {}

	void run(unsigned t_max){
		for(unsigned t = 0; t < t_max; ++t){
			generate_jobs(t);
			if(_jobs.size() > 0){
				schedule(next_job());
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

	void schedule(std::size_t job){
		_jobs[job].c -= 1;
		if(_jobs[job].c == 0){
			_jobs.erase(_jobs.begin() + job);
		}
	}

	bool check_deadlines(){
		bool respected = true;
		for(Job& job : _jobs){
			job.d -= 1;
			if(job.d == 0){
				respected = false;
			}
		}
		return respected;
	}

	virtual std::size_t next_job() = 0;

};


class DMSimulator : public Simulator{

	std::size_t next_job() {
		
	}
};

#endif