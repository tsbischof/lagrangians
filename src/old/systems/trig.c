#include "trig.h"

enum { X, V, M, K1, K2, K3, K4, K5, K6 };

double trig_force(double *r) {
    return(-r[K1]*r[X]-r[K2]*r[V]+r[K3]*cos(r[X])+r[K4]*cos(r[V])+r[K5]*sin(r[X])+r[K6]*sin(r[V]));
}

void trig_integrate(double *r, double dt) {
    verlet(trig_force, r, dt);
}

double trig_first_turnaround(double *r, double *r0, double t, double *values, 
		int done) {
	return(first_turnaround(r, r0, t, values, done));
}

double trig_speed(double *r, double *r0, double t, double *values, 
		int done) {
	return(speed(r, r0, t, values, done));
}
