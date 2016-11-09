#include "task.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::ostream& operator<<(std::ostream& out, const Task& t){
	out << t.o << " " << t.t << " " << t.d << " " << t.c;
	return out;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& vec){
	for(T t: vec){
		out << t << std::endl;
	}
	return out;
}

std::vector<Task> read_tasks(std::ifstream in){
	std::string task_string;
	std::vector<Task> tasks;
	Task t;
	while(! in.eof()){
		in >> t.o >> t.t >> t.d >> t.c;
		tasks.push_back(t);
	}
	return tasks;
}

void sort_by_u(std::vector<Task>& tasks){
	std::sort(tasks.begin(), tasks.end(), [](Task x, Task y){
		return x.u >= y.u;
	});
}

std::vector<std::vector<Task>> partition(unsigned procs, const std::vector<Task>& tasks){
	std::vector<std::vector<Task>> partitioning(procs);
	for(const Task& t: tasks){
		
	}
}



int main() {
	std::vector<Task> tasks = read_tasks(std::ifstream("../test/example"));
	sort_by_u(tasks);
	std::cout << tasks;
	return 0;
}

