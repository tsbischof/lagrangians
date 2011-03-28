#ifndef INTEGRATORS_H
#define INTEGRATORS_H

void verlet(double (*force)(double *), double *, double dt);
void runge_kutta_4(void (*derivs)(double *, double *),
		double *r, double dt, int vec_length);

void vector_sub(double *v1, double *v2, int length);
void vector_copy(double *v1, double *v2, int length);
void vector_div_int(double *v1, int div, int length);

void do_row(double *r_left, double *r_right, int n_variables, double *t_limits,
	int width, void (*integrate)(double *, double),
	double (*rule)(double *, double *, double, double *, int),
	double *result);
double do_single_run_with_rule(double *r, double *r0, int n_variables, 
	double *t_limits, void (*integrate)(double *, double),
	double (*rule)(double *, double *, double, double *, int) );

void advance_image(double ***r, int width, int height, double dt,
	void (*integrate)(double *, double) );

#endif 
