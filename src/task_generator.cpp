#include "generator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]){
  double u_goal;
  int n;
  std::string out_file;

  for (int i = 1; i < argc; i+=2) {
    std::string arg = argv[i];
    if (arg == "-u"){
      u_goal = std::stod(argv[i+1]);
    } else if (arg == "-n"){
      n = std::stoi(argv[i+1]);
    } else if (arg == "-o") {
      out_file = argv[i+1];
    }
  }

  Generator g(u_goal, n, (int)time(0));
  g.create_tasks();
  std::cout << "Utilisation = " << g.u() * 100 << std::endl;
  std::ofstream out;
  out.open(out_file);
  out << g.pprint();
  out.close();

  return EXIT_SUCCESS;
}
