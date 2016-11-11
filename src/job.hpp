#ifndef __JOB_HPP
#define __JOB_HPP

struct Job{
	unsigned t_begin, d, d_tot, c;

	Job(const Task& task, unsigned t);

};


#endif