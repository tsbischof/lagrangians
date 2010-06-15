#include "includes.h"

enum { X, V, M, K1, K2 };

void do_trig(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "1";
	functions.integrate_funcs[0] = integrate_trig;

	functions.integrate_names[1] = "2";
	functions.integrate_funcs[1] = integrate_trig2;

	functions.integrate_names[2] = "3";
	functions.integrate_funcs[2] = integrate_trig3;

	functions.rule_names[0] = "first_turnaround";
	functions.rule_funcs[0] = first_turnaround;
	
	functions.rule_names[1] = "speed";
	functions.rule_funcs[1] = speed;

	char *variable_order[5] = {"x", "v", "m", "k1", "k2"};
	double variable_defaults[5] = {0, 0, 1, 1};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
                5, &functions);
}

double force_trig(double *r) {
	return(4*sin(r[0])-0.1*r[0]);
}

void integrate_trig(double *r, double dt) {
	verlet(force_trig, r, dt);
}

double force_trig2(double *r) {
    return(-r[0]-r[1]+4*cos(r[0])+4*sin(r[1]));
}

void integrate_trig2(double *r, double dt) {
    verlet(force_trig2, r, dt);
}

double force_trig3(double *r) {
    return(sin(r[0])-0.1*r[1]-0.01*r[0]);
}

void integrate_trig3(double *r, double dt) {
    verlet(force_trig3, r, dt);
}

