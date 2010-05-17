#ifndef INSTALLED_HPP_
#define INSTALLED_HPP_

#include <vector>
#include <string>

using namespace std;

void (*get_dispatcher(string, bool *))(string);
void (*get_integrator(string integrator, bool *))(vector<double> *, double);
bool (*get_rule(string rule, bool *))(vector<double> *, vector<double> *);

#endif /* INSTALLED_HPP_ */
