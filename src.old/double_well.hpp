#ifndef DOUBLE_WELL_HPP_
#define DOUBLE_WELL_HPP_

#include <vector>
#include <string>

using namespace std;

double force_double_well(vector<double> *r);
void integrate_double_well(vector<double> *r, double dt);
void do_double_well(string config_filename);

#endif /* DOUBLE_WELL_HPP_ */
