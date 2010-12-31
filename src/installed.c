#include "installed.h"

#include <stdio.h>
#include <string.h>

#include "systems/double_pendulum.h"
#include "systems/harmonic_oscillator.h"
#include "systems/double_well.h"
#include "systems/trig.h"
#include "systems/dangling_stick.h"
#include "systems/springy_pendulum.h"
#include "systems/double_spring.h"
#include "systems/platform_with_springs.h"
#include "systems/driven_damped_pendulum.h"

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
	} else if ( ! strcmp(integrator, "double_spring") ) {
		return(&do_double_spring);
	} else if ( ! strcmp(integrator, "platform_with_springs") ) {
		return(&do_platform_with_springs);
	} else if ( ! strcmp(integrator, "driven_damped_pendulum") ) {
		return(&do_driven_damped_pendulum);
	} else {
		return(NULL);
	}
}
