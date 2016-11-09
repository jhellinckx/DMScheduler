#include <vector>
#include "task.hpp"
#include "processor.hpp"

Processor::Processor() : tasks(), acc_u(0.0) {}

void Processor::add_task(const Task& t){
	acc_u += t.u;
	tasks.push_back(t);
}