#include "generator.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <ctime>

int main(int argc, char* argv[]){
  srand((unsigned)time(NULL));
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

  if (u_goal > 100 * n){
    std::cerr << "Impossible to generate: not enough tasks!" << std::endl;
    return EXIT_FAILURE;
  }

  Generator g(u_goal, n);
  std::cout << "Utilisation = " << g.u() * 100 << std::endl;
  std::ofstream out;
  out.open(out_file);
  out << g.pprint();
  out.close();

  return EXIT_SUCCESS;
}
