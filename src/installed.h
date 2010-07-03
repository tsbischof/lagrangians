#ifndef INSTALLED_H_
#define INSTALLED_H_

#ifdef __cplusplus
extern "C" {
#endif
#include <iniparser.h>
#ifdef __cplusplus
}
#endif 

#include "grapher.h"

void (*get_dispatcher(char *integrator))(dictionary *, Grapher *);

#endif
