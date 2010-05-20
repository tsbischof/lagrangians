#include <vector>
#include <math.h>
#include <iostream>

using namespace std;

void verlet(double (*force)(vector<double> *),  vector<double> *r, double dt) {
	double f = force(r);
	(*r)[0] += (*r)[1]*dt + 1/2*f*pow(dt,2);
	(*r)[1] += f*dt;
	f = force(r);
	(*r)[1] += f*dt;
}

void runge_kutta_4(void (*derivs)(vector<double> *, vector<double> *),
		vector<double> *r, double dt) {
	/* Advance r by dt, using the Runge-Kutta fourth-order method. */
	int vec_length = (*r).size();

	vector<double> dydx(vec_length, 0),	dydxt(vec_length, 0),
			yt(vec_length, 0), k1(vec_length, 0), k2(vec_length, 0),
			k3(vec_length, 0), k4(vec_length, 0);

    derivs(r, &dydx);
    for (int i = 0; i < vec_length; i++) {
        k1[i] = dt * dydx[i];
        yt[i] = (*r)[i] + 0.5 * k1[i];
    }

    derivs(&yt, &dydxt);
    for (int i = 0; i < vec_length; i++) {
        k2[i] = dt * dydxt[i];
        yt[i] = (*r)[i] + 0.5 * k2[i];
    }

    derivs(&yt, &dydxt);
    for (int i = 0; i < vec_length; i++) {
        k3[i] = dt * dydxt[i];
        yt[i] = (*r)[i] + k3[i];
    }

    derivs(&yt, &dydxt);
    for (int i = 0; i < vec_length; i++) {
        k4[i] = dt * dydxt[i];
        (*r)[i] = (*r)[i] + 1.0/6.0*(k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);;
    }
}
