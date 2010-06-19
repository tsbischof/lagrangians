#ifndef SPRINGY_PENDULUM_H_
#define SPRINGY_PENDULUM_H_

#include "includes.h"

void derivs_springy_pendulum(double *r, double *drdt);
void integrate_springy_pendulum(double *r, double dt);
void do_springy_pendulum(dictionary *options, Grapher *grapher);
double U_springy_pendulum(double *);
double T_springy_pendulum(double *);
double upper_flip_springy_pendulum(double *, double *, double, double *, int);
double lower_flip_springy_pendulum(double *, double *, double, double *, int);

#endif
