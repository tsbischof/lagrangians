#ifndef PARSE_UTILS_H_
#define PARSE_UTILS_H_

#include "grapher.h"

int contains(char *list[], char *element);
void parse_plot(char  *line, char *x, char *y);
char* remove_whitespace(char *s);
char* clean(char *s);
void to_lower(char *s);
int index_of(char *list[], char *element);
int parse_limits(FILE *config_file, double *limits, char *name);
int setup_config(Grapher *grapher, char *config_filename,
		char *variable_order[], char *valid_rules[]);
char* name_from_config(char *config_filename);



#endif
