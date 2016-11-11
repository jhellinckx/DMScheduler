#include "job.hpp"

Job::Job(const Task& task, unsigned t) : 
	t_begin(t), d(t + task.d), c(task.c) {}