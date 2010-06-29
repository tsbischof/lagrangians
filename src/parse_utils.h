#ifndef PARSE_UTILS_H_
#define PARSE_UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "grapher.h"
//extern "C" {
	#include <iniparser.h>
//}
#include "rules.h"

int contains(char *list[], char *element);
int parse_plot(char  *line, char **x, char **y);
int parse_limits(char *line, double *limits);
int setup_config(Grapher *grapher, dictionary *options,
		char *variable_order[], double *variable_defaults, int n_vars, 
		Functions *functions);

#endif
