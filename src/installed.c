#include <stdio.h>
#include <string.h>

#include "harmonic_oscillator.h"
#include "double_well.h"
#include "double_pendulum.h"
#include "rules.h"

int get_dispatcher(char *integrator, int (*dispatcher)(char *)) {
	if ( strcmp(integrator, "harmonic_oscillator") ) {
		dispatcher = do_harmonic_oscillator;
		return(0);
	} else if ( strcmp(integrator, "double_well") ) {
		dispatcher = do_double_well;
		return(0);
	} else if ( strcmp(integrator, "double_pendulum") ) {
		dispatcher = do_double_pendulum;
		return(0);
	} else {
		return(1);
	}
}

int get_integrator(char *integrator, 
                   void (*integrator_func)(double *, double)) {
	if ( strcmp(integrator, "harmonic_oscillator") ) {
		integrator_func = integrate_harmonic_oscillator;
		return(0);
	} else if ( strcmp(integrator, "double_well") ) {
		integrator_func = integrate_double_well;
		return(0);
	} else if ( strcmp(integrator, "double_pendulum") ) {
		integrator_func = integrate_double_pendulum;
		return(0);
	} else {
		return(1);
	}
}

int get_rule(char *rule, int (*rule_func)(double *, double *)) {
	if ( strcmp(rule, "first_turnaround") ) {
		rule_func = first_turnaround;
		return(0);
	} else if ( strcmp(rule, "first_flip") ) {
		rule_func = first_flip;
		return(0);
	} else {
		return(1);
	}
}
