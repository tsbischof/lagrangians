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

void do_driven_damped_pendulum(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "sinusoid";
	functions.integrate_funcs[0] = integrate_driven_damped_pendulum_sinusoid;
	functions.integrate_names[1] = "square";
	functions.integrate_funcs[1] = integrate_driven_damped_pendulum_square;

	functions.rule_names[0] = "first_flip";
	functions.rule_funcs[0] = first_flip_driven_damped_pendulum;

	functions.validate = 0;

    char *variable_order[9] = {"phi", "dphi", "m", "b", "g", "l", "a", 
								"omega", "delta"};
	double variable_defaults[8] = {0, 0, 1, 0, 9.8, 1, 1, 1};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
				9, &functions);
}

void derivs_driven_damped_pendulum(double *r, double *drdt) {
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

void derivs_driven_damped_pendulum_sinusoid(double *r, double *drdt) {
	derivs_driven_damped_pendulum(r, drdt);
	drdt[DPHI] += r[A]*cos(r[DELTA]);
}

void integrate_driven_damped_pendulum_sinusoid(double *r, double dt) {
	runge_kutta_4(derivs_driven_damped_pendulum_sinusoid, r, dt, 9);
}

void derivs_driven_damped_pendulum_square(double *r, double *drdt) {
	derivs_driven_damped_pendulum(r, drdt);
	drdt[DPHI] += r[A]*(-1+2*(double)((int)floor(r[DELTA] / r[OMEGA]) % 2));
}

void integrate_driven_damped_pendulum_square(double *r, double dt) {
	runge_kutta_4(derivs_driven_damped_pendulum_square, r, dt, 9);
}

double first_flip_driven_damped_pendulum(double *r, double *r0, 
		double t, double *values, int done) {
	if ( ! done ) {
		return( (r[PHI] > 2*M_PI) || (r[PHI] < -2*M_PI) );
	} else {
		return(t);
	}	
}
