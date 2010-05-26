#include "includes.hpp"
#include "double_well.hpp"

using namespace std;

double force_double_well(double *r) {
	return(pow(r[0],3) * -0.06 + r[0] * 0.2);
}

void integrate_double_well(double *r, double dt) {
	verlet(force_double_well, r, dt);
}

void do_double_well(string config_filename, po::options_description *options) {
	options->add_options()
			("x", "")
			("v", "")
			;

	vector<string> valid_rules;
	valid_rules.push_back("first_turnaround");

	vector<string> variable_order;
	variable_order.push_back("x");
	variable_order.push_back("v");

	vector<double> x_limits(3,0.0);
	vector<double> y_limits(3,0.0);
	vector<double> t_limits(3,0.0);

	string x, y, rule, comment;

	if ( parse_config(config_filename, options, &variable_order,
			&valid_rules, &rule, &x_limits, &y_limits, &t_limits, &x, &y,
			&comment) ) {
		Grapher mygrapher;

		setup_config(&mygrapher, config_filename, &variable_order,
				"double_well", rule, x, &x_limits, y, &y_limits,
				&t_limits, comment);

		mygrapher.do_image();
		mygrapher.to_raw();
		mygrapher.to_ppm();
	}
}
