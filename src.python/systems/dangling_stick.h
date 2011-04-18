#ifndef DANGLING_STICK_H
#define DANGLING_STICK_H

#include "includes.h"

void dangling_stick_derivs(double *r, double *drdt);
void dangling_stick_integrate(double *r, double t, double dt);
double dangling_stick_U(double *);
double dangling_stick_T(double *);
int dangling_stick_lower_flip_energy(double *);
double dangling_stick_lower_flip(double *, double *, double, double *, int);


#endif
