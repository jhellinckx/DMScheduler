#include "generator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]){
  double u_goal;
  int n;
  std::string out_file;

  if (argc != 7){
    return EXIT_FAILURE;
  }

  for (size_t i = 1; i < 7; i+=2) {
    std::string arg = argv[i];
    if (arg == "-u"){
      u_goal = std::stod(argv[i+1]);
    } else if (arg == "-n"){
      n = std::stoi(argv[i+1]);
    } else if (arg == "-o") {
      out_file = argv[i+1];
    } else {
      return EXIT_FAILURE;
    }
  }

  std::cout << u_goal << ' ' << n << std::endl;

  Generator g(u_goal, n);
  std::ofstream out;
  out.open(out_file);
  out << g.pprint();
  out.close();

  return EXIT_SUCCESS;
}
