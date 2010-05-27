#include "includes.h"
#include "double_pendulum.h"

double m1 = 1;
double m2 = 1;
double l1 = 1;
double l2 = 1;
double g = 9.8;

int PHI1 = 0;
int OMEGA1 = 1;
int PHI2 = 2;
int OMEGA2 = 3;

void derivs_double_pendulum(double *r, double *drdt) {
    double delta = r[PHI2] - r[PHI1];
    drdt[PHI1] = r[OMEGA1];
    drdt[OMEGA1] =
    		       ( m2 * l1 * pow(r[OMEGA1],2) * sin(delta) * cos(delta)
                   + m2 * g * sin(r[PHI2]) * cos(delta)
                   + m2 * l2 * pow(r[OMEGA2],2) * sin(delta)
                   - (m1+m2) * g * sin(r[PHI1]))
                   / ((m1+m2) * l1 - m2 * l1 * pow(cos(delta),2));
    drdt[PHI2] = r[OMEGA2];
    drdt[OMEGA2] =
    		       ( -m2 * l2 * pow(r[OMEGA2],2) * sin(delta) * cos(delta)
                   + (m1+m2) * g * sin(r[PHI1]) * cos(delta)
                   - (m1+m2) * l1 * pow(r[OMEGA1],2) * sin(delta)
                   - (m1+m2) * g * sin(r[PHI2]))
                   / (((m1+m2) * l2 - m2 * l2 * pow(cos(delta),2)));
}

void integrate_double_pendulum(double *r, double dt) {
	runge_kutta_4(derivs_double_pendulum, r, dt, 4);
}

int do_double_pendulum(dictionary *options) {
	char *valid_rules = {"first_flip"};
	char *variable_order[4] = {"phi1", "omega1", "phi2", "omega2"};

	Grapher grapher;
	if ( ! setup_config(&grapher, options,
		 &variable_order[0], 4, &valid_rules) ) {
		do_image(&grapher);
		to_raw(&grapher);
		to_ppm(&grapher);
		return(0);
	} else {
		return(1);
	}
}
