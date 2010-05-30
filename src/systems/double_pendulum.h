#ifndef DOUBLE_PENDULUM_H_
#define DOUBLE_PENDULUM_H_

#include "includes.h"

void derivs_double_pendulum(double *r, double *drdt);
void integrate_double_pendulum(double *r, double dt);
void do_double_pendulum(dictionary *options, Grapher *grapher);
double U(double *);
double T(double *);
int energy_double_pendulum(double *);

#endif
