#ifndef __GENERATOR_HPP
#define __GENERATOR_HPP

#include "task.hpp"
#include <vector>
#include <random>

class Generator {
  double utilisation_goal;
  const int n;
  std::vector<Task> tasks;
  std::default_random_engine gen;

public:
  Generator() = default;
  Generator(double u, int n, int seed);
  Generator(double u, int n);
  std::string pprint() const;
  std::vector<Task> create_tasks();

private:
  std::vector<Task> create_dumb();
  void o_shift();
  int rand_between(int lower, int upper);

public:
  bool utilisation_check() const;
  double u() const;
};

#endif
