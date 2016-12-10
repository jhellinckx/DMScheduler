#include "generator.hpp"
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <random>

#define UNI_UPPER_LIMIT 100
#define UNI_LOWER_LIMIT 10
#define O_UPPER_LIMIT 100
#define O_LOWER_LIMIT 0
#define PRECISION 1000.0

int rand_between(int lower, int upper){
  return rand() % (upper - lower + 1) + lower;
}

unsigned gcd(unsigned a, unsigned b) {
    return b == 0 ? a : gcd(b, a % b);
}

void set_c_and_t(double d, Task& t){
  d = round((d - std::floor(d)) * PRECISION);
  const double r = gcd((unsigned)round(d), PRECISION);
  t.t = (unsigned)(PRECISION / r);
  t.c = (unsigned)(d / r);
}

Generator::Generator(double u, int n)
  : utilisation_goal(u/100.0), n(n), tasks(n) {
  create_jobs();
}

void Generator::create_jobs(){
  Task t;
  const double mean = utilisation_goal / (double)n;
  const double dif = fmin(1-mean, mean);
  std::default_random_engine gen;
  std::uniform_real_distribution<double> uni(mean-dif, mean+dif);

  std::vector<double> us(n);
  for(auto it = us.begin(); it != us.end(); ++it){
    *it = -log(uni(gen));
  }
  double s = std::accumulate(us.begin(), us.end(), 0.0);
  for(auto it = us.begin(); it != us.end(); ++it){
    *it = utilisation_goal * *it / s;
  }
  for (size_t i = 0; i < (unsigned)n; ++i) {
    set_c_and_t(us[i], t);
    t.d = rand_between(t.c, t.t);
    t.o = rand_between(O_LOWER_LIMIT, O_UPPER_LIMIT);
    t.u = (double)t.c / t.t;
    tasks[i] = t;
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
