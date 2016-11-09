#include "task.hpp"

Task::Task(){}

Task::Task(unsigned o, unsigned t, unsigned d, unsigned c)
    : o(o), t(t), d(d), c(c), u(((double)c)/t) {}
