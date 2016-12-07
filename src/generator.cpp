#include "generator.hpp"
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iostream>

#define T_UPPER_LIMIT 100
#define T_LOWER_LIMIT 10
#define O_UPPER_LIMIT 100
#define O_LOWER_LIMIT 0

int rand_between(int lower, int upper){
  return rand() % (upper - lower + 1) + lower;
}

Generator::Generator(double u, int n)
  : utilisation_goal(u/100), n(n), tasks(n) {
  create_jobs();
}

void Generator::create_jobs(){
  unsigned t, d, c, o;
  for (size_t i = 0; i < (unsigned)n-1; ++i) {
    do {
      t = rand_between(T_LOWER_LIMIT, T_UPPER_LIMIT);
    } while (1 > t * utilisation_goal / 2);
    c = rand_between(1, (int)round(t * utilisation_goal / 2));
    d = rand_between(c+1, t);
    o = rand_between(O_LOWER_LIMIT, O_UPPER_LIMIT);
    tasks[i] = Task(o, t, d, c);
    utilisation_goal -= (double)c / (double)t;
  }
  t = rand_between(T_LOWER_LIMIT, T_UPPER_LIMIT);
  c = (int)round(t * utilisation_goal);
  d = rand_between(c+1, t);
  tasks[n-1] = Task(0, t, d, c);
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
