#ifndef PROC_HPP
#define PROC_HPP

#include "task.hpp"
#include <ostream>
#include <vector>

class Processor {

  std::vector<Task> tasks;
  double acc_u;

public:
  Processor();
  double u() const;
  std::vector<Task> get_tasks() const;
  void add_task(const Task &t);
};

std::ostream &operator<<(std::ostream &out, const Processor &proc);

#endif
