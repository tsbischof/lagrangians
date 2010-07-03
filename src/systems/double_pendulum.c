#include "double_pendulum.h"

#include <math.h>

/* Simulates the motion of two pendulums, attached to each other. The first
 * pendulum is attached to a fixed point, has a mass m1 attached to its lower
 * end, and has length l1. The second pendulum is fixed to the bottom of the 
 * first, and has mass m2 and length l2. Phi and omega describe the angle and
 * angular velocity of the two angles.
 */

enum { PHI1, DPHI1, PHI2, DPHI2, M1, M2, L1, L2, G };

void do_double_pendulum(dictionary *options, Grapher *grapher) {
    Functions functions;
    functions.integrate_names[0] = "double_pendulum";
    functions.integrate_funcs[0] = integrate_double_pendulum;

    functions.rule_names[0] = "first_flip";
    functions.rule_funcs[0] = first_flip_double_pendulum;
	functions.rule_names[1] = "count_flips";
	functions.rule_funcs[1] = count_flips_double_pendulum;

    functions.validate = 1; // set to 1 to indicate that we do have a validator
    functions.validate_names[0] = "energy";
    functions.validate_funcs[0] = energy_double_pendulum;

    char *variable_order[9] = {"phi1", "dphi1", "phi2", "dphi2", "m1", "m2",
                             	  "l1", "l2", "g"};
    double variable_defaults[9] = {0, 0, 0, 0, 1, 1, 1, 1, 9.8};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
                9, &functions);
}

void derivs_double_pendulum(double *r, double *drdt) {
    drdt[PHI1] = r[DPHI1];
    drdt[DPHI1] = ( - r[G]*((2*r[M1]+r[M2])*sin(r[PHI1])
							+ r[M2]*sin(r[PHI1]-2*r[PHI2]))
						- r[L1]*r[M2]*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI1],2)
						- 2*r[L2]*r[M2]*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2))
				/ (2*r[L1]*(r[M1]+r[M2]-r[M2]*pow(cos(r[PHI1]-r[PHI2]),2)));
    drdt[PHI2] = r[DPHI2];
    drdt[DPHI2] = (sin(r[PHI1]-r[PHI2]) 
					* (r[G]*(r[M1]+r[M2])*cos(r[PHI1])
					   +r[L1]*(r[M1]+r[M2])*pow(r[DPHI1],2)
					   +r[L2]*r[M2]*cos(r[PHI1]-r[PHI2])*pow(r[DPHI2],2)))
					/ (r[L2]*(r[M1]+r[M2]-r[M2]*pow(cos(r[PHI1]-r[PHI2]),2)));
	drdt[M1] = 0;
	drdt[M2] = 0;
	drdt[L1] = 0;
	drdt[L2] = 0;
	drdt[G] = 0;
}

void integrate_double_pendulum(double *r, double dt) {
	runge_kutta_4(derivs_double_pendulum, r, dt, 9);
}

double T_double_pendulum(double *r) {
	return(1.0/2*r[M1]*pow(r[DPHI1]*r[L1], 2) + 
		1.0/2*r[M2]*(pow(r[DPHI1]*r[L1], 2)+pow(r[DPHI2]*r[L2], 2)+
					 2*r[L1]*r[L2]*r[DPHI1]*r[DPHI2]*cos(r[PHI1]-r[PHI2])));
}

double U_double_pendulum(double *r) {
	return(r[M1]*r[G]*r[L1]*(1-cos(r[PHI1]))+r[M2]*r[G]*(r[L1]*(1-cos(r[PHI1]))+r[L2]*(1-cos(r[PHI2]))));
}

int energy_double_pendulum(double *r) {
	double r_min[9];
	r_min[PHI1] = 0;
	r_min[PHI2] = M_PI;
	r_min[DPHI1] = 0;
	r_min[DPHI2] = 0;
	r_min[M1] = r[M1];
	r_min[M2] = r[M2];
	r_min[L1] = r[L1];
	r_min[L2] = r[L2];
	r_min[G] = r[G];

	return( U_double_pendulum(r) + T_double_pendulum(r) 
			> U_double_pendulum(&r_min[0]) );
} 

double first_flip_double_pendulum(double *r, double *r0, 
			double t, double *values, 
				int done) {
	if ( ! done ) {
		return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
	} else {
		return(t);
	}
}

double count_flips_double_pendulum(double *r, double *r0, double t, 
	double *values, int done) {
	if ( ! done ) {
		return(0);
	} else {
		return(r[PHI2]);
	}
}
