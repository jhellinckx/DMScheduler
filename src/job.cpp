#include "job.hpp"
#include <iostream>

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
	return !completed() && t >= d;
}

std::ostream& operator<<(std::ostream& out, const Job& job){
	out << "ID(" << job.task_id << ")" << " D(" << job.d << ")" << " D_rel(" << job.d_rel << ")" << " C(" << job.c << ")";
	out << "Execs : ";
	for(const unsigned& exec: job.executions) { out << exec << " "; }
	return out;
}



