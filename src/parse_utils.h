#ifndef PARSE_UTILS_H_
#define PARSE_UTILS_H_

#include <stdio.h>
#include "grapher.h"
#include "iniparser/iniparser.h"

int contains(char *list[], char *element);
int parse_plot(char  *line, char **x, char **y);
int parse_limits(char *line, double *limits);
int index_of(char *list[], char *element);
int setup_config(Grapher *grapher, dictionary *options,
		char *variable_order[], int n_vars, char *valid_rules[]);
char* name_from_config(char *config_filename);

#endif
