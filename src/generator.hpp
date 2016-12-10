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
  std::string pprint() const;

private:
  void create_jobs();
  void create_dumb();
  void o_shift();
  int rand_between(int lower, int upper);

public:
  bool utilisation_check() const;
  double u() const;
};

#endif
