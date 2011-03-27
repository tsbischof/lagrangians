#include "platform_with_springs.h"

/* Simulates the motion of a (2D) block with length and height, rotating about
 * a fixed axis perpendicular to the plane of the block. At either end is a 
 * spring with a mass attached.
 * r1: length of the left spring
 * r10: natural length of the left spring
 * k1: spring constant of the left spring
 * m1: mass dangling from the left spring
 * phi1: angle of the left spring, relative to vertical
 * r2, r20, k2, m2, phi2: ditto
 * b, h: base and height of the block
 * x0, y0: the origin about which the system rotates
 * m3: mass of the block
 * phi3: angle of the block, relative to vertical
 */

enum { R1, DR1, R10, K1, M1, PHI1, DPHI1, 
		R2, DR2, R20, K2, M2, PHI2, DPHI2,
		B, H, X0, Y0, M3, PHI3, DPHI3, G};

void platform_with_springs_derivs(double *r, double *drdt) {
}

void platform_with_springs_integrate(double *r, double dt) {
	runge_kutta_4(platform_with_springs_derivs, r, dt, 22);
}

double platform_with_springs_U(double *r) {
	return(r[G]*sin(r[PHI3])*r[M3]*(pow(r[X0],2)+pow(r[Y0],2))+r[G]*(1-cos(r[PHI1]))*r[M1]*r[R1]+(r[K1]*pow(-r[R10]+r[R1],2))/2.+r[G]*(1-cos(r[PHI2]))*r[M2]*r[R2]+(r[K2]*pow(-r[R20]+r[R2],2))/2.);
}

double platform_with_springs_T(double *r) {
	return(((((pow(r[B],2)+pow(r[H],2))*r[M3])/12.+r[M3]*(pow(r[X0],2)+pow(r[Y0],2)))*pow(r[PHI3],2))/2.+(r[M2]*(pow(-(cos(r[PHI2])*r[DR2])+sin(r[PHI2])*r[R2]*r[DPHI2]+(r[B]*cos(r[PHI3])*r[DPHI3])/2.,2)+pow(sin(r[PHI2])*r[DR2]+cos(r[PHI2])*r[R2]*r[DPHI2]-(r[B]*sin(r[PHI3])*r[DPHI3])/2.,2)))/2.+(r[M1]*(pow(-(cos(r[PHI1])*r[DR1])+sin(r[PHI1])*r[R1]*r[DPHI1]-(r[B]*cos(r[PHI3])*r[DPHI3])/2.,2)+pow(sin(r[PHI1])*r[DR1]+cos(r[PHI1])*r[R1]*r[DPHI1]+(r[B]*sin(r[PHI3])*r[DPHI3])/2.,2)))/2.);
}

double platform_with_springs_left_flip(double *r, double *r0, 
		double t, double *values, int done) {
	if ( ! done ) {
		return( (r[PHI1] > 2*M_PI) || (r[PHI1] < -2*M_PI) );
	} else {
		return(t);
	}	
}

double platform_with_springs_right_flip(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
    } else {
        return(t);
    }
}

double platform_with_springs_platform_flip(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI3] > 2*M_PI) || (r[PHI3] < -2*M_PI) );
    } else {
        return(t);
    }
}
