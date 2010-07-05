#include "double_spring.h"

#include <math.h>

/* Simulates the motion of two springs, in the same configuration as the 
 * double spring.
 */

enum {PHI1, DPHI1, PHI2, DPHI2, M1, M2, K1, R1, DR1, R10, K2, R2, DR2, R20, G};

void do_double_spring(dictionary *options, Grapher *grapher) {
    Functions functions;
    functions.integrate_names[0] = "double_spring";
    functions.integrate_funcs[0] = integrate_double_spring;

    functions.rule_names[0] = "lower_flip";
    functions.rule_funcs[0] = lower_flip_double_spring;
    functions.rule_names[1] = "upper_flip";
    functions.rule_funcs[1] = upper_flip_double_spring;
	

    functions.validate = 1; // set to 1 to indicate that we do have a validator
    functions.validate_names[0] = "energy";
    functions.validate_funcs[0] = energy_double_spring;

    char *variable_order[15] = {"phi1", "dphi1", "phi2", "dphi2", 
						"m1", "m2",
                        "k1", "r1", "dr1", "r10", 
						"k2", "r2", "dr2", "r20", 
						"g"};
    double variable_defaults[15] = {0, 0, 0, 0, 
						1, 1, 
						1, 1, 0, 1, 
						1, 1, 0, 1,
						9.8};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
                15, &functions);
}

void derivs_double_spring(double *r, double *drdt) {
	drdt[DR1] = (-(r[G]*r[M1])-r[G]*r[M2]+r[G]*r[M1]*cos(r[PHI1])+r[G]*r[M2]*cos(r[PHI1]-r[PHI2])+r[K1]*(r[R10]-r[R1])-cos(r[PHI1]-r[PHI2])*r[K2]*(r[R20]-r[R2])+r[M1]*r[R1]*pow(r[DPHI1],2))/r[M1];
	drdt[DR2] = (-(r[M2]*cos(r[PHI1]-r[PHI2])*r[K1]*(r[R10]-r[R1]))+(r[M1]+r[M2])*r[K2]*(r[R20]-r[R2])+r[M2]*(-2*r[G]*(r[M1]+r[M2])*pow(sin((r[PHI1]-r[PHI2])/2.),2)+r[M1]*r[R2]*pow(r[DPHI2],2)))/(r[M1]*r[M2]);
	drdt[DPHI1] = (-(r[G]*(r[M1]*sin(r[PHI1])+r[M2]*sin(r[PHI1]-r[PHI2])))+sin(r[PHI1]-r[PHI2])*r[K2]*(r[R20]-r[R2])-2*r[M1]*r[DR1]*r[DPHI1])/(r[M1]*r[R1]);
	drdt[DPHI2] = (r[G]*(r[M1]+r[M2])*sin(r[PHI1]-r[PHI2])-sin(r[PHI1]-r[PHI2])*r[K1]*(r[R10]-r[R1])-2*r[M1]*r[DR2]*r[DPHI2])/(r[M1]*r[R2]);
	drdt[R1] = r[DR1];
	drdt[R2] = r[DR2];
	drdt[PHI1] = r[DPHI1];
	drdt[PHI2] = r[DPHI2];
	drdt[G] = 0;
	drdt[K2] = 0;
	drdt[K1] = 0;
	drdt[M1] = 0;
	drdt[M2] = 0;
}

void integrate_double_spring(double *r, double dt) {
	runge_kutta_4(derivs_double_spring, r, dt, 15);
}

double T_double_spring(double *r) {
	return((r[M1]*(pow(-(sin(r[PHI1])*r[DR1])-cos(r[PHI1])*r[R1]*r[DPHI1],2)+pow(cos(r[PHI1])*r[DR1]-sin(r[PHI1])*r[R1]*r[DPHI1],2)))/2.+(r[M2]*(pow(-(sin(r[PHI1])*r[DR1])-sin(r[PHI2])*r[DR2]-cos(r[PHI1])*r[R1]*r[DPHI1]-cos(r[PHI2])*r[R2]*r[DPHI2],2)+pow(cos(r[PHI1])*r[DR1]+cos(r[PHI2])*r[DR2]-sin(r[PHI1])*r[R1]*r[DPHI1]-sin(r[PHI2])*r[R2]*r[DPHI2],2)))/2.);
}

double U_double_spring(double *r) {
	return(r[G]*(r[M1]+r[M2])*(1-cos(r[PHI1]))*r[R1]+(r[K1]*pow(-r[R10]+r[R1],2))/2.+r[G]*r[M2]*(1-cos(r[PHI2]))*r[R2]+(r[K2]*pow(-r[R20]+r[R2],2))/2.);
}

int energy_double_spring(double *r) {
	double r_min[15];
	r_min[PHI1] = 0;
	r_min[PHI2] = M_PI;
	r_min[DPHI1] = 0;
	r_min[DPHI2] = 0;

	r_min[M1] = r[M1];
	r_min[M2] = r[M2];

	r_min[K1] = r[K1];
	r_min[R1] = r[R1];
	r_min[DR1] = r[DR1];
	r_min[R10] = r[R10];

	r_min[K2] = r[K2];
	r_min[R2] = r[R2];
	r_min[DR2] = r[DR2];
	r_min[R20] = r[R20];

	r_min[G] = r[G];

	return( U_double_spring(r) + T_double_spring(r) 
			> U_double_spring(&r_min[0]) );
} 

double lower_flip_double_spring(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
    } else {
        return(t);
    }
}

double upper_flip_double_spring(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI1] > 2*M_PI) || (r[PHI1] < -2*M_PI) );
    } else {
        return(t);
    }
}

