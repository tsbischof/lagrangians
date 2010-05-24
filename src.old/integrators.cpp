#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

void verlet(double (*force)(double *),  double *r, double dt) {
	double f = force(r);
	r[0] += r[1]*dt + 1/2*f*pow(dt,2);
	r[1] += f*dt;
	f = force(r);
	r[1] += f*dt;
}

void runge_kutta_4(void (*derivs)(double *, double *),
		double *r, double dt, int vec_length) {
	/* Advance r by dt, using the Runge-Kutta fourth-order method. */
	double dydx[vec_length], dydxt[vec_length], yt[vec_length], 
		k1[vec_length], k2[vec_length], k3[vec_length], k4[vec_length];

	double *dydxptr, *dydxtptr, *ytptr, *k1ptr, *k2ptr, *k3ptr, *k4ptr;
	dydxptr = dydx;
	dydxtptr = dydxt;
	ytptr = yt;
	k1ptr = k1;
	k2ptr = k2;
	k3ptr = k3;
	k4ptr = k4;

    derivs(r, dydxptr);
    for (int i = 0; i < vec_length; i++) {
        k1[i] = dt * dydx[i];
        yt[i] = r[i] + 0.5 * k1[i];
    }

    derivs(ytptr, dydxtptr);
    for (int i = 0; i < vec_length; i++) {
        k2[i] = dt * dydxt[i];
        yt[i] = r[i] + 0.5 * k2[i];
    }

    derivs(ytptr, dydxtptr);
    for (int i = 0; i < vec_length; i++) {
        k3[i] = dt * dydxt[i];
        yt[i] = r[i] + k3[i];
    }

    derivs(ytptr, dydxtptr);
    for (int i = 0; i < vec_length; i++) {
        k4[i] = dt * dydxt[i];
        r[i] = r[i] + 1.0/6.0*(k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    }
}
