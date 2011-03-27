#include "harmonic_oscillator.h"

enum { X, V, M, K };

double harmonic_oscillator_force(double *r) {
	return(-r[K]*r[X]);
}

void harmonic_oscillator_integrate(double *r, double dt) {
	verlet(harmonic_oscillator_force, r, dt);
}

double harmonic_oscillator_first_turnaround(double *r, double *r0, double t,
    double *values, int done) {
    return(first_turnaround(r, r0, t, values, done));
}

double harmonic_oscillator_speed(double *r, double *r0, double t, 
	double *values, int done) {
    return(speed(r, r0, t, values, done));
}



