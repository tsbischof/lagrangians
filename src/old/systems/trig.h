#ifndef TRIG_H
#define TRIG_H

#include "includes.h"

double trig_force(double *);
void trig_integrate(double *, double);
double trig_first_turnaround(double *r, double *r0, double t, double *values,
        int done);
double trig_speed(double *r, double *r0, double t, double *values,
        int done);



#endif
