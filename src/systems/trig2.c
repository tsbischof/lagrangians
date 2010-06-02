#include "includes.h"
#include <math.h>

double force_trig2(double *r) {
	return(-r[0]-r[1]+4*cos(r[0])+4*sin(r[1]));
}

void integrate_trig2(double *r, double dt) {
	verlet(force_trig2, r, dt);
}

void do_trig2(dictionary *options, Grapher *grapher) {
	char *valid_rules[2] = {"speed", "first_turnaround"};
	char *variable_order[2] = {"x", "v"};
	setup_config(grapher, options, &variable_order[0], 2, &valid_rules[0]);
}
