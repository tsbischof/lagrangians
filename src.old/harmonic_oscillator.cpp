#include "includes.hpp"
#include "harmonic_oscillator.hpp"

using namespace std;

double force_harmonic_oscillator(vector<double> *r) {
	return((*r)[0] * -1);
}

void integrate_harmonic_oscillator(vector<double> *r, double dt) {
	verlet(force_harmonic_oscillator, r, dt);
}

void do_harmonic_oscillator(string config_filename) {
	po::options_description options;
	string comment;
	options.add_options()
			("time,t", "")
			("x", "")
			("v", "")
			("plot", "")
			("rule", "")
			("comment", po::value<string>(&comment)->default_value(""))
			;

	vector<string> valid_rules;
	valid_rules.push_back("first_turnaround");

	vector<string> variable_order;
	variable_order.push_back("x");
	variable_order.push_back("v");

	vector<double> x_limits(3,0.0);
	vector<double> y_limits(3,0.0);
	vector<double> t_limits(3,0.0);

	string x, y, rule;

	if ( parse_config(config_filename, &options, &variable_order,
			&valid_rules, &rule, &x_limits, &y_limits, &t_limits, &x, &y,
			&comment) ) {
		Grapher mygrapher;

		setup_config(&mygrapher, config_filename, &variable_order,
				"harmonic_oscillator", rule, x, &x_limits, y, &y_limits,
				&t_limits, comment);

		mygrapher.do_image();
		mygrapher.to_raw();
		mygrapher.to_ppm();
	}
}
