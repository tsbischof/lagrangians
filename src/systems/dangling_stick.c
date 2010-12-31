#include "dangling_stick.h"

/* Simulates the motion of a massless stick with a mass on either end, attached
 * to a spring. Descriptions of parameters:
 * r: length of the spring
 * phi1: angle of the spring, relative to vertical.
 * phi2: angle of the stick, relative to vertical.
 * m1: mass of the mass connected to the spring
 * m2: mass of the mass dangling from the stick
 * k: spring constant for the spring
 * l: length of the stick
 * A `d' in front of the variable indicates a time derivative.
 */

enum { R, DR, PHI1, DPHI1, PHI2, DPHI2, M1, M2, R0, K, L, G };

void do_dangling_stick(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "dangling_stick";
	functions.integrate_funcs[0] = integrate_dangling_stick;

	functions.rule_names[0] = "first_flip";
	functions.rule_funcs[0] = first_flip_dangling_stick;

	functions.validate = 1;
	functions.validate_names[0] = "energy";
	functions.validate_funcs[0] = energy_dangling_stick;

    char *variable_order[12] = {"r", "dr", "phi1", "dphi1", "phi2", "dphi2",
								"m1", "m2", "r0", "k", "l", "g"};
	double variable_defaults[12] = {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 9.8};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
				12, &functions);
}

void derivs_dangling_stick(double *r, double *drdt) {
	drdt[DR] = (-2*r[G]*pow(r[M1],2)-3*r[G]*r[M1]*r[M2]-r[G]*pow(r[M2],2)+2*r[K]*r[M1]*r[R0]+r[K]*r[M2]*r[R0]+2*r[G]*pow(r[M1],2)*cos(r[PHI1])+2*r[G]*r[M1]*r[M2]*cos(r[PHI1])+r[G]*r[M1]*r[M2]*cos(2*(r[PHI1]-r[PHI2]))+r[G]*pow(r[M2],2)*cos(2*(r[PHI1]-r[PHI2]))-r[K]*r[M2]*r[R0]*cos(2*(r[PHI1]-r[PHI2]))+r[R]*(-(r[K]*(2*r[M1]+r[M2]-r[M2]*cos(2*(r[PHI1]-r[PHI2]))))+2*r[M1]*(r[M1]+r[M2])*pow(r[DPHI1],2))+2*r[L]*r[M1]*r[M2]*cos(r[PHI1]-r[PHI2])*pow(r[DPHI2],2))/(2.*r[M1]*(r[M1]+r[M2]));
	drdt[DPHI1] = -(2*r[G]*pow(r[M1],2)*sin(r[PHI1])+2*r[G]*r[M1]*r[M2]*sin(r[PHI1])+r[G]*r[M1]*r[M2]*sin(2*(r[PHI1]-r[PHI2]))+r[G]*pow(r[M2],2)*sin(2*(r[PHI1]-r[PHI2]))-r[K]*r[M2]*r[R0]*sin(2*(r[PHI1]-r[PHI2]))+r[K]*r[M2]*r[R]*sin(2*(r[PHI1]-r[PHI2]))+4*r[M1]*(r[M1]+r[M2])*r[DR]*r[DPHI1]+2*r[L]*r[M1]*r[M2]*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2))/(2.*r[M1]*(r[M1]+r[M2])*r[R]);
	drdt[DPHI2] = ((r[G]*(r[M1]+r[M2])-r[K]*r[R0]+r[K]*r[R])*sin(r[PHI1]-r[PHI2]))/(r[L]*r[M1]);
	drdt[R] = r[DR];
	drdt[PHI1] = r[DPHI1];
	drdt[PHI2] = r[DPHI2];
	drdt[R0] = 0;
	drdt[G] = 0;
	drdt[K] = 0;
	drdt[L] = 0;
	drdt[M1] = 0;
	drdt[M2] = 0;
}

void integrate_dangling_stick(double *r, double dt) {
	runge_kutta_4(derivs_dangling_stick, r, dt, 12);
}

double U_dangling_stick(double *r) {
	return(r[G]*r[L]*r[M2]*(1-cos(r[PHI2]))+r[G]*(r[M1]+r[M2])*(1-cos(r[PHI1]))*r[R]+(r[K]*pow(-r[R0]+r[R],2))/2.);
}

double T_dangling_stick(double *r) {
	return(((r[M1]+r[M2])*(pow(r[DR],2)+pow(r[R],2)*pow(r[DPHI1],2)))/2.+(r[M2]*(2*r[L]*sin(r[PHI1]-r[PHI2])*r[DR]*r[DPHI2]+2*r[L]*cos(r[PHI1]-r[PHI2])*r[R]*r[DPHI1]*r[DPHI2]+pow(r[L],2)*pow(r[DPHI2],2)))/2.);
}

int energy_dangling_stick(double *r) {
	return(1==1);
	double r_min[11];
	r_min[R] = r[R0];
	r_min[DR] = 0;
	r_min[PHI1] = 0;
	r_min[DPHI1] = 0;
	r_min[PHI2] = M_PI;
	r_min[DPHI2] = 0;
	r_min[M1] = r[M1];
	r_min[M2] = r[M2];
	r_min[R0] = r[R0];
	r_min[K] = r[K];
	r_min[L] = r[L];

	return( U_dangling_stick(r) + T_dangling_stick(r) 
			> U_dangling_stick(&r_min[0]) );
} 

double first_flip_dangling_stick(double *r, double *r0, 
		double t, double *values,
		int done) {
	if ( ! done ) {
		return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
	} else {
		return(t);
	}	
}
