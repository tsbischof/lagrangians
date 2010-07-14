#ifndef PLATFORM_WITH_SPRINGS_H_
#define PLATFORM_WITH_SPRINGS_H_

#include "includes.h"

void derivs_platform_with_springs(double *r, double *drdt);
void integrate_platform_with_springs(double *r, double dt);
void do_platform_with_springs(dictionary *options, Grapher *grapher);
double U_platform_with_springs(double *);
double T_platform_with_springs(double *);
double left_flip_platform_with_springs(double *, double *, double, double *, int);
double right_flip_platform_with_springs(double *, double *, double, double *, int);
double platform_flip_platform_with_springs(double *, double *, double, double *, int);


#endif
