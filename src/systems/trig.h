#ifndef TRIG_H_
#define TRIG_H_

#include "includes.h"

double force_trig(double *r);
void integrate_trig(double *r, double dt);
void do_trig(dictionary *options, Grapher *grapher);

#endif
