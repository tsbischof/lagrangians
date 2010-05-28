#include "includes.h"
#include "harmonic_oscillator.h"

double force_harmonic_oscillator(double *r) {
	return(r[0] * -1);
}

void integrate_harmonic_oscillator(double *r, double dt) {
	verlet(force_harmonic_oscillator, r, dt);
}

void do_harmonic_oscillator(dictionary *options) {
    char *valid_rules = {"first_turnaround"};
    char *variable_order[2] = {"x", "v"};

    Grapher grapher;
    setup_config(&grapher, options, &variable_order[0], 2, &valid_rules);
    do_image(&grapher);
    to_raw(&grapher);
    to_ppm(&grapher);
	exit(0);
}
