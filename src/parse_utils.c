#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "grapher.h"
#include "parse_utils.h"
#include "installed.h"

int contains(char *list[], char *element) {
	int i = 0;
	while ( list[i] != '\0' ) {
		if ( strcmp(list[i], element) ) {
			return(1);
		} else {
			i++;
		}
	}
	return(0);
}

void parse_plot(char  *line, char *x, char *y) {
	sscanf(line, "%s,%s", x, y);
	x = remove_whitespace(x);
	x = remove_whitespace(y);
}

char* remove_whitespace(char *s) {
	int i = 0;
	int j = 0;
	char *out;

	while ( s[i] != '\0' ) {
		if ( s[i] != ' ' ) {
			out[j++] = s[i++];
		}
	}
	return(out);
}

char* clean(char *s) {
	char *out;
	out = remove_whitespace(s);
	to_lower(out);
	return(out);
}

void to_lower(char *s) {
	int i = 0;
	while( s[i] != '\0' ) {
		s[i] = tolower(s[i]);
		i++;
	}
}

int index_of(char *list[], char *element) {
	int i = 0;

	while ( list[i] != '\0' ) {
		if ( strcmp(list[i], element) ) {
			return(i);
		} else {
			i++;
		}
	}
	return(-1);
}

int parse_limits(FILE *config_file, double *limits, char *name) {
	char *pattern;
	sprintf(pattern, "%s = %L,%L,%L\n");
	return(fscanf(config_file, pattern, limits[0], limits[1], limits[2]));
}

int setup_config(Grapher *grapher, char *config_filename,
		char *variable_order[], char *valid_rules[]) {

	FILE *config_file;
	config_file = fopen(config_filename, "r");
	if ( ! config_file ) {
		printf("File %s could not be read.\n", config_filename);
	}

	// Check that the rule is allowed for this simulation.
	char *rule;
	fscanf(config_file, "rule=%s\n", rule);
	rule = clean(rule);

	if ( contains(valid_rules, rule) ) {
		printf("Found valid rule %s.\n", rule);
		if ( get_rule(rule, grapher->rule) ) {
			printf("Installed rule %s.\n", rule);
		} else {
			printf("Fatal: Could not install rule %s.\n", rule);
			return(2);
		}
	} else {
		printf("Fatal: Found invalid rule %s.\n", rule);
		return(1);
	}

	// Determine if x and y are valid variables.
	char *plot, *x, *y;
	fscanf(config_file, "plot = %s\n", plot);
	plot = clean(plot);
	sscanf(plot, "%s,%s", x, y);
	x = clean(x);
	y = clean(y);

	grapher->parm1 = x;
	grapher->parm1_index = index_of(variable_order, x);
	grapher->parm2 = y;
	grapher->parm2_index = index_of(variable_order, y);
	if ( grapher->parm1_index >= 0 || grapher->parm2_index >= 0 ) {
		if ( strcmp(x, y) ) {
			printf("Warning: axes are plotting the same variable %s.\n", x);
		}
		printf("Found valid plot (%s, %s).\n", x, y);
	} else {
		printf("Fatal: Plot parameter %s is invalid.\n", plot);
		return(1);
	}

	// Install the rest of the variables, if present.
	// To do: actually do this.
	double *r0;
	grapher->r0 = r0;
	int i;
	for (i = 0; i < sizeof(variable_order); i++) {
		r0[0] = 0;
	}

	// Do the limits for x, y, t.
	double t_limits[3], x_limits[3], y_limits[3];
	if ( (parse_limits(config_file, &x_limits[0], x) &&
		  parse_limits(config_file, &y_limits[0], y) &&
		  parse_limits(config_file, &t_limits[0], "t") ) ) {
		printf("Found valid limits for %s, %s, t.\n", x, y);
		grapher->parm1_limits = &x_limits[0];
		grapher->parm2_limits = &y_limits[0];
		grapher->t_limits = &t_limits[0];
	} else {
		printf("Found invalid limits for either %s, %s, or t.", x, y);
	}

	// Find the comment, if any.
	char *comment;
	comment = "";
	fscanf(config_file, "comment = %s", comment);
	grapher->comment = comment;

	return(0);
}

char* name_from_config(char *config_filename) {
	char *name;
	if ( sscanf(config_filename, "%s.inp", name) ) {
		return(name);
	} else {
		return(config_filename);
	}
}
