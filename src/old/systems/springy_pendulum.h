#ifndef SPRINGY_PENDULUM_H
#define SPRINGY_PENDULUM_H

#include "includes.h"

void springy_pendulum_derivs(double *r, double *drdt);
void springy_pendulum_integrate(double *r, double dt);
double springy_pendulum_U(double *);
double springy_pendulum_T(double *);
double springy_pendulum_upper_flip(double *, double *, double, double *, int);
int springy_pendulum_upper_flip_energy(double *r);
double springy_pendulum_lower_flip(double *, double *, double, double *, int);
int springy_pendulum_lower_flip_energy(double *r);

#endif
