#include <stdio.h>
#include <string.h>

#include "iniparser/iniparser.h"

#include "systems/harmonic_oscillator.h"
#include "systems/double_well.h"
#include "systems/double_pendulum.h"
#include "systems/trig.h"
#include "systems/trig2.h"
#include "systems/trig3.h"

#include "rules.h"
#include "grapher.h"

void (*get_dispatcher(char *integrator))(dictionary *, Grapher *grapher) {
	if ( ! strcmp(integrator, "harmonic_oscillator") ) {
		return(&do_harmonic_oscillator);
	} else if ( ! strcmp(integrator, "double_well") ) {
		return(&do_double_well);
	} else if ( ! strcmp(integrator, "double_pendulum") ) {
		return(&do_double_pendulum);
	} else if ( ! strcmp(integrator, "trig") ) {
		return(&do_trig);
	} else if ( ! strcmp(integrator, "trig2") ) {
		return(&do_trig2);
	} else if ( ! strcmp(integrator, "trig3") ) {
		return(&do_trig3);
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
	} else if ( ! strcmp(integrator, "trig") ) {
		return(&integrate_trig);
	} else if ( ! strcmp(integrator, "trig2") ) {
		return(&integrate_trig2);
	} else if ( ! strcmp(integrator, "trig3") ) {
		return(&integrate_trig3);
	} else {
		return(NULL);
	}
}

int (*get_rule(char *rule))(double *, double *) {
	if ( ! strcmp(rule, "first_turnaround") ) {
		return(&first_turnaround);
	} else if ( ! strcmp(rule, "first_flip") ) {
		return(&first_flip);
	} else if ( ! strcmp(rule, "speed") ) {
		return(&speed);
	} else {
		return(NULL);
	}
}

int (*get_validator(char *validator))(double *) {
	if ( ! strcmp(validator, "energy_double_pendulum")) {
		return(energy_double_pendulum);
	} else if ( ! strcmp(validator, "all") ) {
		return(validate_all);
	} else {
		return(NULL);
	}
}
