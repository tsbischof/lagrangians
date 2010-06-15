#include "double_pendulum.h"

enum { PHI1, OMEGA1, PHI2, OMEGA2, M1, M2, L1, L2, G };

void do_double_pendulum(dictionary *options, Grapher *grapher) {
    Functions functions;
    functions.integrate_names[0] = "double_pendulum";
    functions.integrate_funcs[0] = integrate_double_pendulum;

    functions.rule_names[0] = "first_flip";
    functions.rule_funcs[0] = first_flip_double_pendulum;

    functions.validate = 1; // set to 1 to indicate that we do have a validator
    functions.validate_names[0] = "energy";
    functions.validate_funcs[0] = energy_double_pendulum;

    char *variable_order[9] = {"phi1", "omega1", "phi2", "omega2", "m1", "m2",
                                "l1", "l2", "g"};
    double variable_defaults[9] = {0, 0, 0, 0, 1, 1, 1, 1, 9.8};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
                9, &functions);
}

void derivs_double_pendulum(double *r, double *drdt) {
    double delta = r[PHI2] - r[PHI1];
    drdt[PHI1] = r[OMEGA1];
    drdt[OMEGA1] =
    		       ( r[M2] * r[L1] * pow(r[OMEGA1],2) * sin(delta) * cos(delta)
                   + r[M2] * r[G] * sin(r[PHI2]) * cos(delta)
                   + r[M2] * r[L2] * pow(r[OMEGA2],2) * sin(delta)
                   - (r[M1] + r[M2]) * r[G] * sin(r[PHI1]))
                   / ((r[M1]+r[M2])*r[L1] - r[M2]*r[L1]*pow(cos(delta),2));
    drdt[PHI2] = r[OMEGA2];
    drdt[OMEGA2] =
    		       ( -r[M2] * r[L2] * pow(r[OMEGA2],2) * sin(delta) * cos(delta)
                   + (r[M1]+r[M2]) * r[G] * sin(r[PHI1]) * cos(delta)
                   - (r[M1]+r[M2]) * r[L1] * pow(r[OMEGA1],2) * sin(delta)
                   - (r[M1]+r[M2]) * r[G] * sin(r[PHI2]))
                   / (((r[M1]+r[M2])*r[L2] - r[M2]*r[L2]*pow(cos(delta),2)));
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
	return(1.0/2*r[M1]*pow(r[OMEGA1], 2) + 1.0/2*r[M2]*pow(r[OMEGA2], 2));
}

double U_double_pendulum(double *r) {
	return(r[M1]*r[G]*r[L1]*(1-cos(r[PHI1]))+r[M2]*r[G]*r[L2]*(2-cos(r[PHI1])-cos(r[PHI2])));
}

int energy_double_pendulum(double *r) {
	double r_min[9];
	r_min[PHI1] = 0;
	r_min[PHI2] = M_PI;
	r_min[OMEGA1] = 0;
	r_min[OMEGA2] = 0;
	r_min[M1] = r[M1];
	r_min[M2] = r[M2];
	r_min[L1] = r[L1];
	r_min[L2] = r[L2];
	r_min[G] = r[G];

	return( U_double_pendulum(r) + T_double_pendulum(r) 
			> U_double_pendulum(&r_min[0]) );
} 

int first_flip_double_pendulum(double *r, double *r0) {
    return ( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
}

