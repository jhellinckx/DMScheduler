#ifndef __GENERATOR_HPP
#define __GENERATOR_HPP

#include "task.hpp"
#include <vector>

class Generator {
  double utilisation_goal;
  const int n;
  std::vector<Task> tasks;

public:
  Generator() = default;
  Generator(double u, int n);
  std::string pprint() const;

private:
  void create_jobs();

public:
  bool utilisation_check() const;
  double u() const;
};

#endif
