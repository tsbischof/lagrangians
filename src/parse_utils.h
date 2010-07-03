#ifndef PARSE_UTILS_H_
#define PARSE_UTILS_H_

#include "grapher.h"

#ifdef __cplusplus
extern "C" {
#endif
#include <iniparser.h>
#ifdef __cplusplus
}
#endif

int contains(char *list[], char *element);
int parse_plot(char  *line, char **x, char **y);
int parse_limits(char *line, double *limits);
int setup_config(Grapher *grapher, dictionary *options,
		char *variable_order[], double *variable_defaults, int n_vars, 
		Functions *functions);

#endif
