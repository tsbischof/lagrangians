#ifndef INTEGRATORS_H_
#define INTEGRATORS_H_

void verlet(double (*force)(double *), double *, double dt);
double runge_kutta_4(void (*derivs)(double *, double *),
		double *r, double dt, int vec_length);

#endif 
