#include "installed.h"

void (*get_dispatcher(char *integrator))(dictionary *, Grapher *grapher) {
	if ( ! strcmp(integrator, "harmonic_oscillator") ) {
		return(&do_harmonic_oscillator);
	} else if ( ! strcmp(integrator, "double_well") ) {
		return(&do_double_well);
	} else if ( ! strcmp(integrator, "double_pendulum") ) {
		return(&do_double_pendulum);
	} else if ( ! strcmp(integrator, "trig") ) {
		return(&do_trig);
	} else if ( ! strcmp(integrator, "dangling_stick") ) {
		return(&do_dangling_stick);
	} else if ( ! strcmp(integrator, "springy_pendulum") ) {
		return(&do_springy_pendulum);
	} else {
		return(NULL);
	}
}
