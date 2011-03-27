#include "driven_damped_pendulum.h"

/* Simulates the motion of a damped pendulum, with a driving force set by the 
 * choice of integrator.
 * Descriptions of parameters:
 * phi: angle of the pendulum
 * m: mass at the end of the pendulum
 * b: damping coefficient
 * g: gravitational constant
 * l: length of the pendulum
 * a: driving amplitude
 * omega: driving angular frequency
 * delta: driving phase
 * A `d' in front of the variable indicates a time derivative.
 */

enum { PHI, DPHI, M, B, G, A, L, OMEGA, DELTA };

void driven_damped_pendulum_derivs(double *r, double *drdt) {
	drdt[PHI] = r[DPHI];
	drdt[DPHI] = -r[G]*sin(r[PHI])-r[B]*r[DPHI]; // damping +r[A]*cos(r[DELTA]);
	drdt[DELTA] = r[OMEGA];
	drdt[M] = 0;
	drdt[B] = 0;
	drdt[G] = 0;
	drdt[A] = 0;
	drdt[L] = 0;
	drdt[DELTA] = 0;
}

void driven_damped_pendulum_sinusoid_derivs(double *r, double *drdt) {
	driven_damped_pendulum_derivs(r, drdt);
	drdt[DPHI] += r[A]*cos(r[DELTA]);
}

void driven_damped_pendulum_sinusoid_integrate(double *r, double dt) {
	runge_kutta_4(driven_damped_pendulum_sinusoid_derivs, r, dt, 9);
}

void driven_damped_pendulum_square_derivs(double *r, double *drdt) {
	driven_damped_pendulum_derivs(r, drdt);
	drdt[DPHI] += r[A]*(-1+2*(double)((int)floor(r[DELTA] / r[OMEGA]) % 2));
}

void driven_damped_pendulum_square_integrate(double *r, double dt) {
	runge_kutta_4(driven_damped_pendulum_square_derivs, r, dt, 9);
}

double driven_damped_pendulum_first_flip(double *r, double *r0, 
		double t, double *values, int done) {
	if ( ! done ) {
		return( (r[PHI] > 2*M_PI) || (r[PHI] < -2*M_PI) );
	} else {
		return(t);
	}	
}
