#include "job.hpp"

Job::Job(const Task& task, unsigned t) : 
	d(t + task.d), d_rel(task.d), c(task.c), task_id(task.id), executions() {}

void Job::execute(unsigned t){
	--c;
	executions.push_back(t);
}

bool Job::completed() const {
	return c == 0;
}

bool Job::missed(unsigned t) const {
	return t >= d;
}