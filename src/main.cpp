#include "processor.hpp"
#include "task.hpp"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iostream>
#include <sstream>
#include <vector>

template <typename T>
std::ostream &operator<<(std::ostream &out, const std::vector<T> &vec) {
  for (T t : vec) {
    out << t << std::endl;
  }
  return out;
}

std::vector<Task> read_tasks(std::ifstream in) {
  std::string task_string;
  std::vector<Task> tasks;
  Task t;
  while (!in.eof()) {
    in >> t.o >> t.t >> t.d >> t.c;
    t.u = ((double)t.c) / t.t;
    tasks.push_back(t);
  }
  return tasks;
}

void sort_by_u(std::vector<Task> &tasks) {
  std::sort(tasks.begin(), tasks.end(),
            [](Task x, Task y) { return x.u >= y.u; });
}

std::vector<Processor> partition(unsigned procs,
                                 const std::vector<Task> &tasks) {
  std::vector<Processor> partitioning(procs);
  for (const Task &t : tasks) {
    std::size_t proc(0);
    double min_diff(2);
    bool placed(false);
    for (std::size_t i(0); i < tasks.size(); ++i) {
      if (partitioning[i].u() + t.u <= 1.0 &&
          fabs(t.u - (1.0 - partitioning[i].u())) < min_diff) {
        min_diff = fabs(t.u - (1.0 - partitioning[i].u()));
        std::cout << i << " " << min_diff << std::endl;
        placed = true;
        proc = i;
      }
    }
    if (placed) {
      partitioning[proc].add_task(t);
    } else {
      std::cout << "No Partition here" << std::endl;
      break;
    }
  }
  return partitioning;
}

int main() {
  std::vector<Task> tasks = read_tasks(std::ifstream("../test/example"));
  sort_by_u(tasks);
  std::cout << tasks << std::endl;
  std::cout << partition(4, tasks) << std::endl;
  return 0;
}
