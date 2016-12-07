#ifndef __JOB_HPP
#define __JOB_HPP

#include <vector>
#include "task.hpp"

struct Job{
	unsigned d, d_rel, c;
	unsigned task_id;
	std::vector<unsigned> executions;

	Job() = default;
	Job(const Task& task, unsigned t);

	void execute(unsigned t);
	bool completed() const;
	bool missed(unsigned t) const;

};


#endif