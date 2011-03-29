#include <math.h>
#include <omp.h>
#include <stdlib.h>
#include "integrators.h"

const int SCRATCH_SIZE = 100;
enum { NOT_DONE, DONE };

void verlet(double (*force)(double *),  double *r, double dt) {
/* Integrates the coordinates by the leapfrog (Verlet) method. Assumes r[0]
 * contains the position, r[1] the velocity, and r[2] the mass.
 */
	double a = force(r)/r[2];
	r[0] += r[1]*dt + 1/2*a*pow(dt,2);
	r[1] += a*dt/2;
	a = force(r)/r[2];
	r[1] += a*dt/2;
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

	int i;

    derivs(r, dydxptr);
    for (i = 0; i < vec_length; i++) {
        k1[i] = dt * dydx[i];
        yt[i] = r[i] + 0.5 * k1[i];
    }

    derivs(ytptr, dydxtptr);
    for (i = 0; i < vec_length; i++) {
        k2[i] = dt * dydxt[i];
        yt[i] = r[i] + 0.5 * k2[i];
    }

    derivs(ytptr, dydxtptr);
    for (i = 0; i < vec_length; i++) {
        k3[i] = dt * dydxt[i];
        yt[i] = r[i] + k3[i];
    }

    derivs(ytptr, dydxtptr);
    for (i = 0; i < vec_length; i++) {
        k4[i] = dt * dydxt[i];
        r[i] = r[i] + 1.0/6.0*(k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
    }
}

void vector_add_mul(double *v1, double *v2, double *dv, int n, int length) {
/* Do v2+n*dv, store as v1 */
	int i;
	for (i = 0; i < length; i++) {
		v1[i] = v2[i] + n*dv[i];
	}
}

void vector_add(double *v1, double *v2, int length) {
/* Adds v1 to v2, and stores the result in v1. */
	int i; 
	for (i = 0; i < length; i++) {
		v1[i] += v2[i];
	}
}

void vector_sub(double *v1, double *v2, int length) {
/* Subtracts v2 from v1, stores the result in v1. */
	int i;
	for (i = 0; i < length; i++) {
		v1[i] -= v2[i];
	}
}

void vector_copy(double *v1, double *v2, int length) {
/* copies v2 to v1. */
	int i; 
	for (i = 0; i < length; i++) {
		v1[i] = v2[i];
	}
}

void vector_div_int(double *v, int div, int length) {
	int i;
	for (i = 0; i < length; i++) {
		v[i] = v[i]/div;
	}
}

void do_row(double *r_left, double *r_right, int n_variables, double *t_limits,
		int width, void (*integrate)(double *, double),
		double (*rule)(double *, double *, double, double *, int), 
		double *result) {
	double *r0, *r_scratch;
	double *dr = (double *)calloc(n_variables, sizeof(double));

	int i;

	vector_copy(dr, r_right, n_variables);
	vector_sub(dr, r_left, n_variables);
	vector_div_int(dr, width, n_variables);	

// this is imperfect; we may have to call this function many times, and each
// time it has to reallocate the memory. 
#pragma omp parallel private(r_scratch, r0)
	{
		r_scratch = (double *)calloc(n_variables, sizeof(double));
		r0 = (double *)calloc(n_variables, sizeof(double));
#pragma omp for
		for (i = 0; i < width; i++) {
			result[i] = do_single_run_with_rule(i, r_left, r_scratch, r0, dr, 
				n_variables, t_limits, integrate, rule);
		}
		free(r_scratch);
		free(r0);
	}
	free(dr);
}

double do_single_run_with_rule(int index, double *r_left, double *r, 
		double *r0, double *dr,
		int n_variables, double *t_limits, void (*integrate)(double *, double),
    	double (*rule)(double *, double *, double, double *, int) ) {
	double t, dt, t_limit;
	double scratch[SCRATCH_SIZE];
	double result;

	t = t_limits[0];
	dt = t_limits[1];
	t_limit = t_limits[2];

	vector_add_mul(r, r_left, dr, index, n_variables);
	vector_add_mul(r0, r_left, dr, index, n_variables);	

	while (t < t_limit) {
		if ( rule(r, r0, t, &scratch[0], NOT_DONE) ) {
			break;
		} else {
			integrate(r, dt);
			t += dt;
		}
	}
	result = rule(r, r0, t, &scratch[0], DONE);
	return(result); 
}

void advance_image(double ***r, int width, int height, double dt,
		void (*integrate)(double *, double) ) {
	int i, j;

	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			integrate(&r[i][j][0], dt);
		}
	}
}
