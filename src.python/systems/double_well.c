#include "double_well.h"

enum { X, V, M, K1, K2 };

double double_well_force(double *r) {
	return(pow(r[X],3) * -r[K1] + r[X] * r[K2]);
}

void double_well_integrate(double *r, double dt) {
	verlet(double_well_force, r, dt);
}

double double_well_first_turnaround(double *r, double *r0, double t, 
	double *values, int done) {
	return(first_turnaround(r, r0, t, values, done));
}

double double_well_speed(double *r, double *r0, double t, double *values, 
	int done) {
	return(speed(r, r0, t, values, done));
}

