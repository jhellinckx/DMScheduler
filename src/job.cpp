#include "job.hpp"

Job::Job(const Task& task, unsigned t) : 
	t_begin(t), d(task.d), d_tot(task.d), c(task.c) {}