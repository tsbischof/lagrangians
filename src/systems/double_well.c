#include "includes.h"
#include "double_well.h"

double force_double_well(double *r) {
	return(pow(r[0],3) * -0.06 + r[0] * 0.6);
}

void integrate_double_well(double *r, double dt) {
	verlet(force_double_well, r, dt);
}

void do_double_well(dictionary *options, Grapher *grapher) {
    char *valid_rules = {"first_turnaround"};
    char *variable_order[2] = {"x", "v"};
	setup_config(grapher, options, &variable_order[0], 2, &valid_rules);
}
