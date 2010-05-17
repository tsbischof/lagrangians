#ifndef PARSE_UTILS_HPP_
#define PARSE_UTILS_HPP_

#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include "grapher.hpp"
using namespace std;
namespace po = boost::program_options;

bool contains(vector<string> *list, string element);
void parse_plot(string line, string *x, string *y);
void remove_whitespace(string *s);
double stod(string s);
bool parse_limits(string line, vector<double> *limits, string name);
bool parse_config(string config_filename, po::options_description *options,
		vector<string> *valid_variables, vector<string> *valid_rules,
		string *rule,
		vector<double> *x_limits, vector<double> *y_limits,
		vector<double> *t_limits, string *x, string *y, string *comment);

string name_from_config(string config_filename);

void setup_config(Grapher *mygrapher, string config_filename,
		vector<string> *variable_order,
		string integrator, string rule,
		string x, vector<double> *x_limits,
		string y, vector<double> *y_limits,
		vector<double> *t_limits, string comment);

#endif /* PARSE_UTILS_HPP_ */
