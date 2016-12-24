#ifndef DOUBLE_PENDULUM_H
#define DOUBLE_PENDULUM_H

#include "includes.h"

void double_pendulum_derivs(double *r, double *drdt);
void double_pendulum_integrate(double *r, double dt);
double double_pendulum_U(double *);
double double_pendulum_T(double *);
int double_pendulum_lower_flip_energy(double *);
double double_pendulum_lower_flip(double *, double *, double, double *, int);
double double_pendulum_lower_turnaround(double *, double *, 
		double, double *, int);

#endif
