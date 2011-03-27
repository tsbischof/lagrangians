#include "double_pendulum.h"

#include <math.h>

/* Simulates the motion of two pendulums, attached to each other. The first
 * pendulum is attached to a fixed point, has a mass m1 attached to its lower
 * end, and has length l1. The second pendulum is fixed to the bottom of the 
 * first, and has mass m2 and length l2. Phi and omega describe the angle and
 * angular velocity of the two angles.
 */

enum { PHI1, DPHI1, PHI2, DPHI2, M1, M2, L1, L2, G };

void double_pendulum_derivs(double *r, double *drdt) {
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

void double_pendulum_integrate(double *r, double dt) {
	runge_kutta_4(double_pendulum_derivs, r, dt, 9);
}

double double_pendulum_T(double *r) {
	return(1.0/2*r[M1]*pow(r[DPHI1]*r[L1], 2) + 
		1.0/2*r[M2]*(pow(r[DPHI1]*r[L1], 2)+pow(r[DPHI2]*r[L2], 2)+
					 2*r[L1]*r[L2]*r[DPHI1]*r[DPHI2]*cos(r[PHI1]-r[PHI2])));
}

double double_pendulum_U(double *r) {
	return(r[M1]*r[G]*r[L1]*(1-cos(r[PHI1]))+r[M2]*r[G]*(r[L1]*(1-cos(r[PHI1]))+r[L2]*(1-cos(r[PHI2]))));
}

int double_pendulum_lower_flip_energy(double *r) {
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

	return( double_pendulum_U(r) + double_pendulum_T(r) 
			> double_pendulum_U(&r_min[0]) );
} 

double double_pendulum_lower_flip(double *r, double *r0, 
			double t, double *values, 
				int done) {
	if ( ! done ) {
		return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
	} else {
		return(t);
	}
}
