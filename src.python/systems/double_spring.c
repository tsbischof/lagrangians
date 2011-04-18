#include "double_spring.h"

#include <math.h>

/* Simulates the motion of two springs, in the same configuration as the 
 * double spring.
 */

enum {PHI1, DPHI1, PHI2, DPHI2, M1, M2, K1, R1, DR1, R1_0, 
										K2, R2, DR2, R2_0, G};

void double_spring_derivs(double *r, double *drdt) {
	drdt[DR1] = (-(r[G]*r[M1])-r[G]*r[M2]+r[G]*r[M1]*cos(r[PHI1])+r[G]*r[M2]*cos(r[PHI1]-r[PHI2])+r[K1]*(r[R1_0]-r[R1])-cos(r[PHI1]-r[PHI2])*r[K2]*(r[R2_0]-r[R2])+r[M1]*r[R1]*pow(r[DPHI1],2))/r[M1];
	drdt[DR2] = (-(r[M2]*cos(r[PHI1]-r[PHI2])*r[K1]*(r[R1_0]-r[R1]))+(r[M1]+r[M2])*r[K2]*(r[R2_0]-r[R2])+r[M2]*(-2*r[G]*(r[M1]+r[M2])*pow(sin((r[PHI1]-r[PHI2])/2.),2)+r[M1]*r[R2]*pow(r[DPHI2],2)))/(r[M1]*r[M2]);
	drdt[DPHI1] = (-(r[G]*(r[M1]*sin(r[PHI1])+r[M2]*sin(r[PHI1]-r[PHI2])))+sin(r[PHI1]-r[PHI2])*r[K2]*(r[R2_0]-r[R2])-2*r[M1]*r[DR1]*r[DPHI1])/(r[M1]*r[R1]);
	drdt[DPHI2] = (r[G]*(r[M1]+r[M2])*sin(r[PHI1]-r[PHI2])-sin(r[PHI1]-r[PHI2])*r[K1]*(r[R1_0]-r[R1])-2*r[M1]*r[DR2]*r[DPHI2])/(r[M1]*r[R2]);
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

void double_spring_integrate(double *r, double t, double dt) {
	runge_kutta_4(double_spring_derivs, r, dt, 15);
}

double double_spring_T(double *r) {
	return((r[M1]*(pow(-(sin(r[PHI1])*r[DR1])-cos(r[PHI1])*r[R1]*r[DPHI1],2)+pow(cos(r[PHI1])*r[DR1]-sin(r[PHI1])*r[R1]*r[DPHI1],2)))/2.+(r[M2]*(pow(-(sin(r[PHI1])*r[DR1])-sin(r[PHI2])*r[DR2]-cos(r[PHI1])*r[R1]*r[DPHI1]-cos(r[PHI2])*r[R2]*r[DPHI2],2)+pow(cos(r[PHI1])*r[DR1]+cos(r[PHI2])*r[DR2]-sin(r[PHI1])*r[R1]*r[DPHI1]-sin(r[PHI2])*r[R2]*r[DPHI2],2)))/2.);
}

double double_spring_U(double *r) {
	return(r[G]*(r[M1]+r[M2])*(1-cos(r[PHI1]))*r[R1]+(r[K1]*pow(-r[R1_0]+r[R1],2))/2.+r[G]*r[M2]*(1-cos(r[PHI2]))*r[R2]+(r[K2]*pow(-r[R2_0]+r[R2],2))/2.);
}

double double_spring_lower_flip(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
    } else {
        return(t);
    }
}

int double_spring_lower_flip_energy(double *r) {
	double r_min[15];
	r_min[PHI1] = 0;
	r_min[PHI2] = M_PI;
	r_min[DPHI1] = 0;
	r_min[DPHI2] = 0;

	r_min[M1] = r[M1];
	r_min[M2] = r[M2];

	r_min[K1] = r[K1];
	r_min[R1] = r[R1_0];
	r_min[DR1] = 0;
	r_min[R1_0] = r[R1_0];

	r_min[K2] = r[K2];
	r_min[R2] = r[R2_0];
	r_min[DR2] = 0;
	r_min[R2_0] = r[R2_0];

	r_min[G] = r[G];

	return( double_spring_U(r) + double_spring_T(r) 
			> double_spring_U(&r_min[0]) );
} 

double double_spring_upper_flip(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI1] > 2*M_PI) || (r[PHI1] < -2*M_PI) );
    } else {
        return(t);
    }
}

int double_spring_upper_flip_energy(double *r) {
	double r_min[15];
	r_min[PHI1] = M_PI;
	r_min[PHI2] = 0;
	r_min[DPHI1] = 0;
	r_min[DPHI2] = 0;

	r_min[M1] = r[M1];
	r_min[M2] = r[M2];

	r_min[K1] = r[K1];
	r_min[R1] = r[R1_0];
	r_min[DR1] = 0;
	r_min[R1_0] = r[R1_0];

	r_min[K2] = r[K2];
	r_min[R2] = r[R2_0];
	r_min[DR2] = 0;
	r_min[R2_0] = r[R2_0];

	r_min[G] = r[G];

	return( double_spring_U(r) + double_spring_T(r) 
			> double_spring_U(&r_min[0]) );
} 

