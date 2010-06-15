#include "harmonic_oscillator.h"

enum { X, V, M, K };
double force_harmonic_oscillator(double *r) {
	return(-r[K]*r[X]);
}

void integrate_harmonic_oscillator(double *r, double dt) {
	verlet(force_harmonic_oscillator, r, dt);
}

void do_harmonic_oscillator(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "harmonic_oscillator";
	functions.integrate_funcs[0] = integrate_harmonic_oscillator;

	functions.rule_names[0] = "first_turnaround";
	functions.rule_funcs[0] = first_turnaround;

	functions.rule_names[1] = "speed";
	functions.rule_funcs[1] = speed;


    char *variable_order[4] = {"x", "v", "m", "k"};
	double variable_defaults[4] = {0, 0, 1, 1};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
				4, &functions);
}
