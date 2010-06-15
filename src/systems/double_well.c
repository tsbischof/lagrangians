#include "double_well.h"

enum { X, V, M, K1, K2 };

void do_double_well(dictionary *options, Grapher *grapher) {
    Functions functions;
    functions.integrate_names[0] = "double_well";
    functions.integrate_funcs[0] = integrate_double_well;

    functions.rule_names[0] = "first_turnaround"; 
	functions.rule_funcs[0] = first_turnaround;

	functions.rule_names[1] = "speed";
    functions.rule_funcs[1] = speed;

    char *variable_order[5] = {"x", "v", "m", "k1", "k2"};
    double variable_defaults[5] = {0, 0, 1, 0.06, 0.6};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
                5, &functions);
}

double force_double_well(double *r) {
	return(pow(r[X],3) * -r[K1] + r[X] * r[K2]);
}

void integrate_double_well(double *r, double dt) {
	verlet(force_double_well, r, dt);
}
