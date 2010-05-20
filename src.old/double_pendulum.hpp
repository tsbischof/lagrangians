#ifndef DOUBLE_PENDULUM_HPP_
#define DOUBLE_PENDULUM_HPP_

#include <vector>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

void derivs_double_pendulum(vector<double> *r, vector<double> *drdt);
void integrate_double_pendulum(vector<double> *r, double dt);
void do_double_pendulum(string config_filename,
		po::options_description *options);

#endif /* DOUBLE_PENDULUM_HPP_ */
