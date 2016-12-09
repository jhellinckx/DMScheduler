#include "generator.hpp"
#include <numeric>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <sstream>

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
  double remaining_ub((double)n * (pow(2.0, (1.0/(double)n) - 1)));
  unsigned t, d, c, o;
  for (size_t i = 0; i < (unsigned)n-1; ++i) {
    t = rand_between(T_LOWER_LIMIT, T_UPPER_LIMIT);
    c = rand_between(1, (int)round(t * utilisation_goal / 2));
    d = rand_between((int)round(2.0 * c / remaining_ub), t);
    o = rand_between(O_LOWER_LIMIT, O_UPPER_LIMIT);
    tasks[i] = Task(o, t, d, c);
    utilisation_goal -= (double)c / (double)t;
    remaining_ub -= (double)c / (double)d;
  }
  t = rand_between(T_LOWER_LIMIT, T_UPPER_LIMIT);
  c = (int)round(t * utilisation_goal);
  d = (int)round((double)c / remaining_ub);
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

bool Generator::ub_check() const{
  double fst = std::accumulate(tasks.begin(), tasks.end(), 0.0,
      [](double acc, Task t){return acc + ((double)t.c / (double)t.d);});
  double nd = (double)n;
  double snd = nd * (pow(2.0, (1.0/nd) - 1));
  return fst <= snd;
}

double Generator::u() const{
  return std::accumulate(tasks.begin(), tasks.end(), 0.0,
      [](double acc, Task t){return acc + t.u;});
}
