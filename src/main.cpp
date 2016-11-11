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
  Task task;
  while (!in.eof()) {
    in >> task.o >> task.t >> task.d >> task.c;
    task.u = ((double)task.c) / task.t;
    tasks.push_back(task);
  }
  return tasks;
}

void decreasing_utilization(std::vector<Task> &tasks) {
  std::sort(tasks.begin(), tasks.end(),
            [](Task x, Task y) { return x.u >= y.u; });
}

std::vector<std::vector<Task>> partition_tasks(std::size_t partitions,
                                 const std::vector<Task> &tasks) {
  std::vector<std::vector<Task>> partitioning(partitions);
  std::vector<double> utilization(partitions, 0.0);
  for (const Task &task : tasks) {
    std::size_t partition = 0;
    double min_diff = 1.0;
    bool placed = false;
    for (std::size_t i = 0; i < partitions; ++i) {
      if (utilization[i] + task.u <= 1.0 &&
          fabs(task.u - (1.0 - utilization[i])) < min_diff) {
        min_diff = fabs(task.u - (1.0 - utilization[i]));
        placed = true;
        partition = i;
      }
    }
    if (placed) {
      partitioning[partition].push_back(task);
      utilization[partition] += task.u;
    } else {
      std::cout << "No Partition here" << std::endl;
      break;
    }
  }
  return partitioning;
}

void print_partitions(const std::vector<std::vector<Task>>& partitions){
	for(const std::vector<Task>& tasks : partitions){
		double acc_u = 0.0;
		std::cout << "Partition : ";
		for(const Task& task : tasks){
			std::cout << task << " / ";
			acc_u += task.u;
		}
		std::cout << "u = " << acc_u << std::endl;
	}
}

int main() {
  std::vector<Task> tasks = read_tasks(std::ifstream("../test/example"));
  decreasing_utilization(tasks);
  print_partitions(partition_tasks(4, tasks));
  return 0;
}
