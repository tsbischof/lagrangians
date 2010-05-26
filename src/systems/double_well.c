#include "includes.h"
#include "double_well.h"

double force_double_well(double *r) {
	return(pow(r[0],3) * -0.06 + r[0] * 0.2);
}

void integrate_double_well(double *r, double dt) {
	verlet(force_double_well, r, dt);
}

int do_double_well(char *config_filename) {
    char *valid_rules = {"first_turnaround"};
    char *variable_order[2] = {"x", "v"};

	Grapher grapher;
	if ( ! setup_config(&grapher, config_filename,
                      &variable_order[0], &valid_rules) ) {
        do_image(&grapher);
        to_raw(&grapher);
        to_ppm(&grapher);
        return(0);
    } else {
        return(1);
    }

}
