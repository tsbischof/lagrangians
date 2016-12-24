#ifndef PLATFORM_WITH_SPRINGS_H
#define PLATFORM_WITH_SPRINGS_H

#include "includes.h"

void platform_with_springs_derivs(double *r, double *drdt);
void platform_with_springs_integrate(double *r, double dt);
double platform_with_springs_U(double *);
double platform_with_springs_T(double *);
double platform_with_springs_left_flip(double *, double *, double, 
	double *, int);
double platform_with_springs_right_flip(double *, double *, double, 
	double *, int);
double platform_with_springs_platform_flip(double *, double *, double, 
	double *, int);


#endif
