#ifndef DOUBLE_WELL_H
#define DOUBLE_WELL_H

#include "includes.h"

double double_well_force(double *r);
void double_well_integrate(double *r, double dt);
double double_well_first_turnaround(double *r, double *r0, double t,
    double *values, int done);
double double_well_speed(double *r, double *r0, double t, double *values,
    int done);



#endif 
