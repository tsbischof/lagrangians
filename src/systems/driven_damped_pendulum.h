#ifndef DRIVEN_DAMPED_PENDULUM_H_
#define DRIVEN_DAMPED_PENDULUM_H_

#include "includes.h"

void do_driven_damped_pendulum(dictionary *options, Grapher *grapher);
void derivs_driven_damped_pendulum(double *r, double *drdt);
void derivs_driven_damped_pendulum_sinusoid(double *r, double *drdt);
void integrate_driven_damped_pendulum_sinusoid(double *r, double dt);
void derivs_driven_damped_pendulum_square(double *r, double *drdt);
void integrate_driven_damped_pendulum_square(double *r, double dt);
double first_flip_driven_damped_pendulum(double *, double *, double, double *, int);


#endif
