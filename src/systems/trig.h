#ifndef TRIG_H_
#define TRIG_H_

#include "includes.h"

void do_trig(dictionary *options, Grapher *grapher);
double force_trig(double *r);
void integrate_trig(double *r, double dt);
double force_trig2(double *r);
void integrate_trig2(double *r, double dt);
double force_trig3(double *r);
void integrate_trig3(double *r, double dt);
double force_trigfull(double *);
void integrate_trigfull(double *, double);

#endif
