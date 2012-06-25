#ifndef DRIVEN_DAMPED_PENDULUM_H_
#define DRIVEN_DAMPED_PENDULUM_H_

#include "includes.h"

void driven_damped_pendulum_derivs(double *r, double *drdt);
void driven_damped_pendulum_sinusoid_derivs(double *r, double *drdt);
void driven_damped_pendulum_sinusoid_integrate(double *r, double dt);
void driven_damped_pendulum_square_derivs(double *r, double *drdt);
void driven_damped_pendulum_square_integrate(double *r, double dt);
double driven_damped_pendulum_first_flip(double *, double *, double, 
	double *, int);


#endif
