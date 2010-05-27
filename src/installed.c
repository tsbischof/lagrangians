#include <stdio.h>
#include <string.h>

#include "iniparser/iniparser.h"
#include "harmonic_oscillator.h"
#include "double_well.h"
#include "double_pendulum.h"
#include "rules.h"

int (*get_dispatcher(char *integrator))(dictionary *) {
	if ( ! strcmp(integrator, "harmonic_oscillator") ) {
		return(&do_harmonic_oscillator);
	} else if ( ! strcmp(integrator, "double_well") ) {
		return(&do_double_well);
	} else if ( ! strcmp(integrator, "double_pendulum") ) {
		return(&do_double_pendulum);
	} else {
		return(NULL);
	}
}

void (*get_integrator(char *integrator))(double *, double) {
	if ( ! strcmp(integrator, "harmonic_oscillator") ) {
		return(&integrate_harmonic_oscillator);
	} else if ( ! strcmp(integrator, "double_well") ) {
		return(&integrate_double_well);
	} else if ( ! strcmp(integrator, "double_pendulum") ) {
		return(&integrate_double_pendulum);
	} else {
		return(NULL);
	}
}

int (*get_rule(char *rule))(double *, double *) {
	if ( ! strcmp(rule, "first_turnaround") ) {
		return(&first_turnaround);
	} else if ( ! strcmp(rule, "first_flip") ) {
		return(&first_flip);
	} else {
		return(NULL);
	}
}
