#include "includes.hpp"
#include "harmonic_oscillator.hpp"

using namespace std;

double force_harmonic_oscillator(double *r) {
	return(r[0] * -1);
}

void integrate_harmonic_oscillator(double *r, double dt) {
	verlet(force_harmonic_oscillator, r, dt);
}

int do_harmonic_oscillator(char *config_filename) {
    char *valid_rules = {"first_turnaround"};
    char *variable_order[2] = {"x", "v"};

    Grapher grapher;
    if ( parse_config(&grapher, config_filename,
                      &variable_order, &valid_rules) ) {
        do_image(&grapher);
        to_raw(&grapher);
        to_ppm(&grapher);
        return(0);
    } else {
        return(1);
    }
}
