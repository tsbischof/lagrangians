#include "trig.h"

enum { X, V, M, K1, K2, K3, K4, K5, K6 };

void do_trig(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "1";
	functions.integrate_funcs[0] = integrate_trig;

	functions.integrate_names[1] = "2";
	functions.integrate_funcs[1] = integrate_trig2;

	functions.integrate_names[2] = "3";
	functions.integrate_funcs[2] = integrate_trig3;

	functions.integrate_names[3] = "full";
	functions.integrate_funcs[3] = integrate_trigfull;

	functions.rule_names[0] = "first_turnaround";
	functions.rule_funcs[0] = first_turnaround;
	
	functions.rule_names[1] = "speed";
	functions.rule_funcs[1] = speed;

	char *variable_order[9] = {"x", "v", "m", "k1", "k2", "k3", 
								"k4", "k5", "k6"};
	double variable_defaults[9] = {0, 0, 1, 4, 4, 0.01, 0.01, 0.01, 0.01};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
                9, &functions);
}

double force_trig(double *r) {
	return(r[K4]*sin(r[0])-r[K1]*r[0]);
}

void integrate_trig(double *r, double dt) {
	verlet(force_trig, r, dt);
}

double force_trig2(double *r) {
    return(-r[K1]*r[0]-r[K2]*r[1]+r[K3]*cos(r[0])+r[K4]*sin(r[1]));
}

void integrate_trig2(double *r, double dt) {
    verlet(force_trig2, r, dt);
}

double force_trig3(double *r) {
    return(r[K4]*sin(r[0])-r[K2]*r[1]-r[K1]*r[0]);
}

void integrate_trig3(double *r, double dt) {
    verlet(force_trig3, r, dt);
}

double force_trigfull(double *r) {
    return(-r[K1]*r[X]-r[K2]*r[V]+r[K3]*cos(r[X])+r[K4]*cos(r[V])+r[K5]*sin(r[X])+r[K6]*sin(r[V]));
}

void integrate_trigfull(double *r, double dt) {
    verlet(force_trigfull, r, dt);
}
