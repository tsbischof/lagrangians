#ifndef INTEGRATORS_HPP_
#define INTEGRATORS_HPP_

#include <vector>
#include <math.h>
#include <iostream>

void verlet(double (*force)(double *), double *, double dt);
double runge_kutta_4(void (*derivs)(double *, double *),
		double *r, double dt, int vec_length);

#endif /* INTEGRATORS_HPP_ */
