#include "includes.hpp"
#include "double_pendulum.hpp"

#define m1 1
#define m2 1
#define l1 1
#define l2 1
#define g 9.8
#define PI 3.14159265

#define PHI1 0
#define OMEGA1 1
#define PHI2 2
#define OMEGA2 3

using namespace std;

void derivs_double_pendulum(vector<double> *r, vector<double> *drdt) {
    double delta = (*r)[PHI2] - (*r)[PHI1];
    (*drdt)[PHI1] = (*r)[OMEGA1];
    (*drdt)[OMEGA1] =
    		       ( m2 * l1 * pow((*r)[OMEGA1],2) * sin(delta) * cos(delta)
                   + m2 * g * sin((*r)[PHI2]) * cos(delta)
                   + m2 * l2 * pow((*r)[OMEGA2],2) * sin(delta)
                   - (m1+m2) * g * sin((*r)[PHI1]))
                   / ((m1+m2) * l1 - m2 * l1 * pow(cos(delta),2));
    (*drdt)[PHI2] = (*r)[OMEGA2];
    (*drdt)[OMEGA2] =
    		       ( -m2 * l2 * pow((*r)[OMEGA2],2) * sin(delta) * cos(delta)
                   + (m1+m2) * g * sin((*r)[PHI1]) * cos(delta)
                   - (m1+m2) * l1 * pow((*r)[OMEGA1],2) * sin(delta)
                   - (m1+m2) * g * sin((*r)[PHI2]))
                   / (((m1+m2) * l2 - m2 * l2 * pow(cos(delta),2)));
}

void integrate_double_pendulum(vector<double> *r, double dt) {
	runge_kutta_4(derivs_double_pendulum, r, dt);
}

void do_double_pendulum(string config_filename, po::options_description *options) {
	options->add_options()
			("phi1", "")
			("omega1", "")
			("phi2", "")
			("omega2", "")
			;

	vector<string> valid_rules;
	valid_rules.push_back("first_flip");

	vector<string> variable_order;
	variable_order.push_back("phi1");
	variable_order.push_back("omega1");
	variable_order.push_back("phi2");
	variable_order.push_back("omega2");

	vector<double> x_limits(3,0.0);
	vector<double> y_limits(3,0.0);
	vector<double> t_limits(3,0.0);

	string x, y, rule, comment;

	if ( parse_config(config_filename, options, &variable_order,
			&valid_rules, &rule, &x_limits, &y_limits, &t_limits, &x, &y,
			&comment) ) {
		Grapher mygrapher;

		setup_config(&mygrapher, config_filename, &variable_order,
				"double_pendulum", rule, x, &x_limits, y, &y_limits,
				&t_limits, comment);

		mygrapher.do_image();
		mygrapher.to_raw();
		mygrapher.to_ppm();
	}
}
