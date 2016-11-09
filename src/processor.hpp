#ifndef PROC_HPP
#define PROC_HPP

#include <vector>
#include "task.hpp"

class Processor{

	std::vector<Task> tasks;
	double acc_u;

public:
	Processor(){}

	void add_task(const Task& t);

};

#endif
