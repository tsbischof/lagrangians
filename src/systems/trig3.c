#include "includes.h"
#include <math.h>

double force_trig3(double *r) {
	return(sin(r[0])-0.1*r[1]-0.01*r[0]);
}

void integrate_trig3(double *r, double dt) {
	verlet(force_trig3, r, dt);
}

void do_trig3(dictionary *options, Grapher *grapher) {
	char *valid_rules[2] = {"speed", "first_turnaround"};
	char *variable_order[2] = {"x", "v"};
	setup_config(grapher, options, &variable_order[0], 2, &valid_rules[0]);
}
