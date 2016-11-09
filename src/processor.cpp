#include "processor.hpp"
#include "task.hpp"
#include <vector>

Processor::Processor() : tasks(), acc_u(0.0) {}

double Processor::u() const { return acc_u; }

void Processor::add_task(const Task &t) {
  acc_u += t.u;
  tasks.push_back(t);
}

std::vector<Task> Processor::get_tasks() const { return tasks; }

std::ostream &operator<<(std::ostream &out, const Processor &proc) {
  out << "Processor: usage = " << proc.u() << std::endl;
  for (Task t : proc.get_tasks()) {
    out << t << " -- ";
  }
  return out;
}
