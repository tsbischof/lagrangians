#ifndef INSTALLED_H_
#define INSTALLED_H_

#include "iniparser/iniparser.h"

void (*get_dispatcher(char *integrator))(dictionary *);
void (*get_integrator(char *integrator))(double *, double);
int (*get_rule(char *rule))(double *, double *);
int (*get_validator(char *validator))(double *);

#endif
