#ifndef INSTALLED_H_
#define INSTALLED_H_

#include <stdio.h>
#include <string.h>

#include "iniparser/iniparser.h"
#include "rules.h"
#include "grapher.h"

#include "systems/double_pendulum.h"
#include "systems/harmonic_oscillator.h"
#include "systems/double_well.h"
#include "systems/trig.h"
#include "systems/dangling_stick.h"
#include "systems/springy_pendulum.h"

void (*get_dispatcher(char *integrator))(dictionary *, Grapher *);

#endif
