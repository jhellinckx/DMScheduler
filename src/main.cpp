#include "task.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

std::ostream& operator<<(std::ostream& out, const Task& t){
	out << t.o << " " << t.t << " " << t.d << " " << t.c;
	return out;
}

std::vector<Task> read_tasks(std::ifstream in){
	std::string task_string;
	std::vector<Task> tasks;
	Task t;
	while(! in.eof()){
		in >> t.o >> t.t >> t.d >> t.c;
		tasks.push_back(t);
		std::cout << t << std::endl;
	}
	return tasks;
}



int main() {
	std::vector<Task> tasks = read_tasks(std::ifstream("../test/example"));
	return 0;
}

