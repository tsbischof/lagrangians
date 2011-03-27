#ifndef DOUBLE_SPRING_H
#define DOUBLE_SPRING_H

#include "includes.h"

void double_spring_derivs(double *r, double *drdt);
void double_spring_integrate(double *r, double dt);
double double_spring_U(double *);
double double_spring_T(double *);
int double_spring_lower_flip_energy(double *);
double double_spring_lower_flip(double *r, double *r0,
        double t, double *values, int done);
int double_spring_upper_flip_energy(double *);
double double_spring_upper_flip(double *r, double *r0,
        double t, double *values, int done);

#endif
