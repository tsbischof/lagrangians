#ifndef INSTALLED_HPP_
#define INSTALLED_HPP_

#include <vector>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

void (*get_dispatcher(string, bool *))(string, po::options_description *);
void (*get_integrator(string integrator, bool *))(vector<double> *, double);
bool (*get_rule(string rule, bool *))(vector<double> *, vector<double> *);

#endif /* INSTALLED_HPP_ */
