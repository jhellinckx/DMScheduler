#ifndef TASK_HPP
#define TASK_HPP

#include <ostream>

struct Task {
  unsigned o, t, d, c;
  double u;

  Task();
  Task(unsigned, unsigned, unsigned, unsigned);
};

std::ostream &operator<<(std::ostream &out, const Task &t);

#endif
