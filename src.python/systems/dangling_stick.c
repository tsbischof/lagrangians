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

void dangling_stick_derivs(double *r, double *drdt) {
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

void dangling_stick_integrate(double *r, double dt) {
	runge_kutta_4(dangling_stick_derivs, r, dt, 12);
}

double dangling_stick_U(double *r) {
	return(r[G]*r[L]*r[M2]*(1-cos(r[PHI2]))+r[G]*(r[M1]+r[M2])*(1-cos(r[PHI1]))*r[R]+(r[K]*pow(-r[R0]+r[R],2))/2.);
}

double dangling_stick_T(double *r) {
	return(((r[M1]+r[M2])*(pow(r[DR],2)+pow(r[R],2)*pow(r[DPHI1],2)))/2.+(r[M2]*(2*r[L]*sin(r[PHI1]-r[PHI2])*r[DR]*r[DPHI2]+2*r[L]*cos(r[PHI1]-r[PHI2])*r[R]*r[DPHI1]*r[DPHI2]+pow(r[L],2)*pow(r[DPHI2],2)))/2.);
}

int dangling_stick_lower_flip_energy(double *r) {
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

	return( dangling_stick_U(r) + dangling_stick_U(r) 
			> dangling_stick_U(&r_min[0]) );
} 

double dangling_stick_lower_flip(double *r, double *r0, 
		double t, double *values,
		int done) {
	if ( ! done ) {
		return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
	} else {
		return(t);
	}	
}
