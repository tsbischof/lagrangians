#ifndef DANGLING_STICK_H_
#define DANGLING_STICK_H_

#include "includes.h"

void derivs_dangling_stick(double *r, double *drdt);
void integrate_dangling_stick(double *r, double dt);
void do_dangling_stick(dictionary *options, Grapher *grapher);
double U_dangling_stick(double *);
double T_dangling_stick(double *);
int energy_dangling_stick(double *);


#endif
