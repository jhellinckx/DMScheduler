#include "generator.hpp"
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>
#include <ctime>

#define UNI_UPPER_LIMIT 100
#define O_UPPER_LIMIT 100
#define O_LOWER_LIMIT 0
#define PRECISION 200.0

int Generator::rand_between(int lower, int upper){
  std::uniform_int_distribution<unsigned> uni(lower, upper);
  return uni(gen);
}

unsigned gcd(unsigned a, unsigned b) {
    return b == 0 ? a : gcd(b, a % b);
}

void set_c_and_t(double d, Task& t){
  d = round((d - std::floor(d)) * PRECISION);
  const double r = gcd((unsigned)round(d), PRECISION);
  unsigned maybe_t = (unsigned)(PRECISION / r);
  unsigned maybe_c = (unsigned)(d / r);
  t.t = (maybe_t <= 1) ? 2 : maybe_t;
  t.c = (maybe_c == 0) ? 1 : maybe_c;
}

Generator::Generator(double u, int n, int seed)
  : utilisation_goal(u/100.0), n(n), tasks(n), gen(seed) {
  if (utilisation_goal > n){
    std::cout << "Warning: cannot satisfy conditions -> setting u to 100 * n..." << std::endl;
    utilisation_goal = n;
  }
}

Generator::Generator(double u, int n) : Generator(u, n, (int) time(0)) {}

std::vector<Task> Generator::create_tasks(){
  Task t;
  const double mean = utilisation_goal / (double)n;
  const double dif = fmin(1-mean, mean);
  std::uniform_real_distribution<double> uni(mean-dif, mean+dif);

  std::vector<double> us(n);
  for(auto it = us.begin(); it != us.end(); ++it){
    *it = uni(gen);
  }
  double s = std::accumulate(us.begin(), us.end(), 0.0);
  if (s == n){
    return create_dumb();
  }
  for(auto it = us.begin(); it != us.end(); ++it){
    *it = utilisation_goal * *it / s;
  }
  unsigned id = 1;
  for (size_t i = 0; i < (unsigned)n; ++i) {
    set_c_and_t(us[i], t);
    t.d = rand_between(t.c+1, t.t);
    t.o = rand_between(O_LOWER_LIMIT, O_UPPER_LIMIT);
    t.u = (double)t.c / t.t;
    t.id = id++;
    tasks[i] = t;
  }
  o_shift();
  return tasks;
}

std::vector<Task> Generator::create_dumb(){
  std::uniform_int_distribution<unsigned> uni(1, UNI_UPPER_LIMIT);
  for (size_t i = 0; i < (unsigned)n; ++i) {
    unsigned r = uni(gen);
    tasks[i] = Task(uni(gen), r, r, r);
  }
  o_shift();
  return tasks;
}

void Generator::o_shift(){
  const unsigned m = std::min_element(tasks.begin(), tasks.end(), [](Task t, Task s) {return t.o <= s.o;})->o;
  for(auto it = tasks.begin(); it != tasks.end(); ++it){
    it->o -= m;
  }
}

std::string Generator::pprint() const{
  std::ostringstream stream;
  for (auto it = tasks.begin(); it != tasks.end(); ++it) {
    stream << it->o << ' ' << it->t << ' ' << it->d << ' ' << it->c << std::endl;
  }
  return stream.str();
}

bool Generator::utilisation_check() const{
  double u_sum = std::accumulate(tasks.begin(), tasks.end(), 0.0,
      [](double acc, Task t){return acc + t.u;});
  return u_sum > utilisation_goal - 1 && u_sum < utilisation_goal + 1;
}

double Generator::u() const{
  return std::accumulate(tasks.begin(), tasks.end(), 0.0,
      [](double acc, Task t){return acc + t.u;});
}
