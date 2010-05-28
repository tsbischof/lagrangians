#ifndef HARMONIC_OSCILLATOR_H_
#define HARMONIC_OSCILLATOR_H_

#include "includes.h"

double force_harmonic_oscillator(double *r);
void integrate_harmonic_oscillator(double *r, double dt);
void do_harmonic_oscillator(dictionary *options);

#endif 
