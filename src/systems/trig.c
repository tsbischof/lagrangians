#include "includes.h"
#include <math.h>

double force_trig(double *r) {
	return(4*sin(r[0])-0.1*r[0]);
}

void integrate_trig(double *r, double dt) {
	verlet(force_trig, r, dt);
}

void do_trig(dictionary *options, Grapher *grapher) {
	char *valid_rules[2] = {"speed", "first_turnaround"};
	char *variable_order[2] = {"x", "v"};
	setup_config(grapher, options, &variable_order[0], 2, &valid_rules[0]);
}
