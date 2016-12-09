#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <iostream>
#include <sstream>
#include <vector>

#include "task.hpp"
#include "simulator.hpp"
#include "utils.hpp"

std::vector<Task> read_tasks(std::ifstream in) {
  std::string task_string;
  std::vector<Task> tasks;
  Task task;
  unsigned id = 1;
  while (in.peek() != EOF) {
    in >> task.o >> task.t >> task.d >> task.c;
	in.get(); // Consume \n
    task.u = ((double)task.c) / task.t;
    task.id = id++;
    tasks.push_back(task);
  }
  return tasks;
}

int main() {
  std::vector<Task> tasks = read_tasks(std::ifstream("../test/example2"));
  GDMSimulator gdm(tasks, 2);
  gdm.run();
  std::cout << gdm.stringify_simulation() << std::endl;
  gdm.prettify_simulation("schedule.png");
  return 0;
}
