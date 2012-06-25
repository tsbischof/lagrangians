#include "springy_pendulum.h"

/* Simulates the motion of a pendlum with a mass on a spring attached. The 
 * pendulum arm has no mass, but a mass is attached to the end.
 * r: length of the spring
 * phi1: angle of the pendulum, relative to vertical.
 * phi2: angle of the spring, relative to vertical.
 * m1: mass of the mass connected to the pendulum
 * m2: mass of the mass connected to the spring
 * k: spring constant for the spring
 * l: length of the stick
 * A `d' in front of the variable indicates a time derivative.
 */

enum { R, DR, PHI1, DPHI1, PHI2, DPHI2, M1, M2, R_0, K, L, G };

void springy_pendulum_derivs(double *r, double *drdt) {
	drdt[DR] = -r[G]+(r[K]*r[R_0])/r[M2]+r[G]*cos(r[PHI2])+r[L]*cos(r[PHI1]-r[PHI2])*r[DPHI1]*r[DPHI2]+r[R]*(-(r[K]/r[M2])+pow(r[DPHI2],2));
	drdt[DPHI1] = (r[L]*r[M2]*pow(cos(r[PHI1]-r[PHI2]),2)*r[DR]*r[DPHI1]-r[M2]*pow(r[R],2)*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2)-(r[R]*(2*r[G]*r[M1]*sin(r[PHI1])+r[G]*r[M2]*sin(r[PHI1])+r[G]*r[M2]*sin(r[PHI1]-2*r[PHI2])+r[L]*r[M2]*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI1],2)-2*r[M2]*cos(r[PHI1]-r[PHI2])*r[DR]*r[DPHI2]))/2.)/(r[L]*(r[M1]+r[M2]-r[M2]*pow(cos(r[PHI1]-r[PHI2]),2))*r[R]);
	drdt[DPHI2] = (-2*r[L]*(r[M1]+r[M2])*cos(r[PHI1]-r[PHI2])*r[DR]*r[DPHI1]+r[M2]*pow(r[R],2)*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI2],2)+r[R]*(2*r[G]*(r[M1]+r[M2])*cos(r[PHI1])*sin(r[PHI1]-r[PHI2])+2*r[L]*(r[M1]+r[M2])*sin(r[PHI1]-r[PHI2])*pow(r[DPHI1],2)-(4*r[M1]+3*r[M2]-r[M2]*cos(2*(r[PHI1]-r[PHI2])))*r[DR]*r[DPHI2]))/(2.*(r[M1]+r[M2]-r[M2]*pow(cos(r[PHI1]-r[PHI2]),2))*pow(r[R],2));
	drdt[R] = r[DR];
	drdt[PHI1] = r[DPHI1];
	drdt[PHI2] = r[DPHI2];
	drdt[R_0] = 0;
	drdt[G] = 0;
	drdt[K] = 0;
	drdt[L] = 0;
	drdt[M1] = 0;
	drdt[M2] = 0;
}

void springy_pendulum_integrate(double *r, double dt) {
	runge_kutta_4(springy_pendulum_derivs, r, dt, 12);
}

double springy_pendulum_U(double *r) {
	return(r[G]*r[L]*(r[M1]+r[M2])*(1-cos(r[PHI1]))+r[G]*r[M2]*(1-cos(r[PHI2]))*r[R]+(r[K]*pow(-r[R_0]+r[R],2))/2.);
}

double springy_pendulum_T(double *r) {
	return((pow(r[L],2)*(r[M1]+r[M2])*pow(r[DPHI1],2))/2.+(r[M2]*(pow(r[DR],2)+2*r[L]*cos(r[PHI1]-r[PHI2])*r[R]*r[DPHI1]*r[DPHI2]+pow(r[R],2)*pow(r[DPHI2],2)))/2.);
}

int springy_pendulum_lower_flip_energy(double *r) {
    double r_min[12];
    r_min[PHI1] = 0;
    r_min[PHI2] = 0;
    r_min[DPHI1] = M_PI;
    r_min[DPHI2] = 0;

    r_min[M1] = r[M1];
    r_min[M2] = r[M2];

    r_min[K] = r[K];
    r_min[R] = r[R_0];
    r_min[DR] = 0;
    r_min[R_0] = r[R_0];

	r_min[L] = r[L];
    r_min[G] = r[G];

    return( springy_pendulum_U(r) + springy_pendulum_T(r)
            > springy_pendulum_U(&r_min[0]) );
}

double springy_pendulum_lower_flip(double *r, double *r0, 
		double t, double *values, int done) {
	return(flipped(r[PHI2], t, done));
}

int springy_pendulum_upper_flip_energy(double *r) {
    double r_min[12];
    r_min[PHI1] = M_PI;
    r_min[PHI2] = 0;
    r_min[DPHI1] = 0;
    r_min[DPHI2] = 0;

    r_min[M1] = r[M1];
    r_min[M2] = r[M2];

    r_min[K] = r[K];
    r_min[R] = r[R_0];
    r_min[DR] = 0;
    r_min[R_0] = r[R_0];

	r_min[L] = r[L];
    r_min[G] = r[G];

    return( springy_pendulum_U(r) + springy_pendulum_T(r)
            > springy_pendulum_U(&r_min[0]) );
}

double springy_pendulum_upper_flip(double *r, double *r0,
        double t, double *values, int done) {
	return(flipped(r[PHI1], t, done));
}

