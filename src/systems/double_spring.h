#ifndef DOUBLE_SPRING_H_
#define DOUBLE_SPRING_H_

#include "includes.h"

void derivs_double_spring(double *r, double *drdt);
void integrate_double_spring(double *r, double dt);
void do_double_spring(dictionary *options, Grapher *grapher);
double U_double_spring(double *);
double T_double_spring(double *);
int energy_double_spring(double *);
double lower_flip_double_spring(double *r, double *r0,
        double t, double *values, int done);
double upper_flip_double_spring(double *r, double *r0,
        double t, double *values, int done);

#endif
