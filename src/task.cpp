#include "task.hpp"
#include <iostream>

Task::Task() : o(0), t(0), d(0), c(0), u(0.0) {}

Task::Task(unsigned o, unsigned t, unsigned d, unsigned c)
    : o(o), t(t), d(d), c(c), u(((double)c) / t) {}

std::ostream &operator<<(std::ostream &out, const Task &t) {
  out << t.o << " " << t.t << " " << t.d << " " << t.c;
  return out;
}
