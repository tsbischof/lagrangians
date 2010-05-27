#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "iniparser/iniparser.h"
#include "grapher.h"
#include "parse_utils.h"
#include "installed.h"

const char *delims = " ,";

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

int parse_plot(char *plot, char **x, char **y) {
	char *token;
	token = strtok(plot, delims);
	*x = token;
	token = strtok(NULL, delims);
	*y = token;
	if ( y == NULL ) {
		return(1);
	} else {
		return(0);
	}
}	

int parse_limits(char *line, double *limits) {
	char *token;
	int i;
	
	token = strtok(line, delims);
	for (i = 0; i < 3; i++) {
		if ( token == NULL ) {
			return(1);
		} else {
			limits[i] = atof(token);
			token = strtok(NULL, delims);
		}
	}	
	return(0);
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

int setup_config(Grapher *grapher, dictionary *options,
		char *variable_order[], int n_vars, char *valid_rules[]) {

	printf("Setting up the grapher.\n");

	char *integrator;
	integrator = iniparser_getstring(options, ":integrator", "");
	grapher->integrate = get_integrator(integrator);
	printf("Installed integrator %s.\n", integrator);

	// Check that the rule is allowed for this simulation.
	char *rule;
	rule = iniparser_getstring(options, ":rule", "");

	if ( contains(valid_rules, rule) ) {
		grapher->rule = get_rule(rule);
		if ( grapher->rule != NULL ) {
			printf("Installed rule %s.\n", rule);
		} else {
			printf("Fatal: Could not install valid rule %s.\n", rule);
			return(2);
		}
	} else {
		printf("Fatal: Found invalid rule %s.\n", rule);
		return(1);
	}

	// Determine if x and y are valid variables.
	char *plot, *x, *y;
	plot = iniparser_getstring(options, ":plot", "");
	parse_plot(plot, &x, &y);

	if ( contains(variable_order, x) && contains(variable_order, y) ) {
		grapher->parm1 = x;
		grapher->parm1_index = index_of(variable_order, x);
		grapher->parm2 = y;
		grapher->parm2_index = index_of(variable_order, y);
	} else {
		printf("Invalid plot %s with variables %s and %s.\n", plot, x, y);
	}

	if ( grapher->parm1_index >= 0 || grapher->parm2_index >= 0 ) {
		printf("Found valid plot (%s, %s).\n", x, y);

		if ( ! strcmp(x, y) ) {
			printf("Warning: axes are plotting the same variable %s.\n", x);
		}
	} else {
		printf("Fatal: Plot parameter %s is invalid.\n", plot);
		return(1);
	}

	// Install the rest of the variables, if present.
	// To do: actually do this.
	double r0[n_vars];
	int i;
	for (i = 0; i < n_vars; i++) {
		r0[i] = 0;
	}
	grapher->r0 = r0;
	grapher->r0_length = n_vars;

	// Do the limits for x, y, t.
	double t_limits[3], x_limits[3], y_limits[3];
	char *t_lims, *x_lims, *y_lims;
	char x_key[100], y_key[100];
	sprintf(x_key, ":%s", x);
	sprintf(y_key, ":%s", y);
	t_lims = iniparser_getstring(options, ":t", "");
	x_lims = iniparser_getstring(options, x_key, "");
	y_lims = iniparser_getstring(options, y_key, "");

	if ( (! parse_limits(x_lims, &x_limits[0])) &&
		 (! parse_limits(y_lims, &y_limits[0])) &&
		 (! parse_limits(t_lims, &t_limits[0])) ) {
		printf("Found valid limits for %s, %s, t.\n", x, y);
		grapher->parm1_limits = x_limits;
		grapher->parm2_limits = y_limits;
		grapher->t_limits = t_limits;
	} else {
		printf("Found invalid limits for either %s, %s, or t.", x, y);
	}

	// Find the comment, if any.
	grapher->comment = iniparser_getstring(options, ":comment", "");
	
	// The options have been parsed, free the space.
	iniparser_freedict(options);

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
