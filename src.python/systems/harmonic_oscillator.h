#ifndef HARMONIC_OSCILLATOR_H
#define HARMONIC_OSCILLATOR_H

#include "includes.h"

double harmonic_oscillator_force(double *r);
void harmonic_oscillator_integrate(double *r, double dt);
double harmonic_oscillator_first_turnaround(double *r, double *r0, double t,
    double *values, int done);
double harmonic_oscillator_speed(double *r, double *r0, double t, 
	double *values, int done);

#endif 
