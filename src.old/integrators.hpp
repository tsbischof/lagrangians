#ifndef INTEGRATORS_HPP_
#define INTEGRATORS_HPP_

#include <vector>

void verlet(double (*force)(vector<double> *), vector<double> *, double dt);
double runge_kutta_4(void (*derivs)(vector<double> *, vector<double> *),
		vector<double> *r, double dt);

#endif /* INTEGRATORS_HPP_ */
