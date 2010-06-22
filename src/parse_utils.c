#include "parse_utils.h"

const char *delims = " ,";

int contains(char *list[], char *element) {
	int i = 0;
	while ( list[i] != '\0' ) {
		if ( ! strcmp(list[i], element) ) {
			return(i);
		} else {
			i++;
		}
	}
	return(-1);
}

int parse_plot(char *plot, char **x, char **y) {
	char *token;
	token = strtok(plot, delims);
	*x = malloc(sizeof(token));
	strcpy(*x, token);
	token = strtok(NULL, delims);
	*y = malloc(sizeof(token));
	strcpy(*y, token);
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

int setup_config(Grapher *grapher, dictionary *options,
		char *variable_order[], double *variable_defaults, int n_vars, 
		Functions *functions) {
	int i;
	printf("--------------------------------------\n");
	printf("Setting up the grapher.\n");
	char *integrator;
	integrator = iniparser_getstring(options, ":integrator", "");
	if ( ! strcmp("", integrator) ) {
		integrator = iniparser_getstring(options, ":system", "");
	}

	if ( (i = contains(functions->integrate_names, integrator)) >= 0 ) {
		grapher->integrate = functions->integrate_funcs[i];
		if ( grapher->integrate != NULL ) {
			printf("Installed integrator %s.\n", integrator);
		} else {
			printf("Fatal: Could not install valid integrator %s.\n", 
					integrator);
			exit(2);
		}
	} else {
		printf("Fatal: Found invalid integrator %s.\n", integrator);
		exit(1);
	}

	// Check that the rule is allowed for this simulation.
	char *rule;
	rule = iniparser_getstring(options, ":rule", "");

	if ( (i = contains(functions->rule_names, rule)) >= 0 ) {
		grapher->rule = functions->rule_funcs[i];
		if ( grapher->rule != NULL ) {
			printf("Installed rule %s.\n", rule);
		} else {
			printf("Fatal: Could not install valid rule %s.\n", rule);
			exit(2);
		}
	} else {
		printf("Fatal: Found invalid rule %s.\n", rule);
		exit(1);
	}

	char *validator;
	validator = iniparser_getstring(options, ":validator", "all");
	if ( ! strcmp(validator, "all") ) {
		printf("Installed validator validate_all.\n");
		grapher->validate = validate_all;
	} else {
		if ( functions->validate && 
			 (i = contains(functions->validate_names, validator)) >= 0 ) {
			grapher->validate = functions->validate_funcs[i];
			if ( grapher->validate != NULL ) { 
				printf("Found validator %s.\n", validator);
			} else { 
				printf("Fatal: Could not install valid validator %s.\n", 
						validator);
				exit(2);
			}
		} else {
			printf("Fatal: Found invalid validator %s.\n",
					validator);
			exit(1);
		}
	}	

	// Determine if x and y are valid variables.
	char *plot;
	plot = iniparser_getstring(options, ":plot", "");
	parse_plot(plot, &grapher->parm1, &grapher->parm2);

	if ( (grapher->parm1_index = 
			contains(variable_order, grapher->parm1)) >= 0 
		  &&
		 (grapher->parm2_index = 
			contains(variable_order, grapher->parm2)) >= 0) {
		; // both are reasonable indices
	} else {
		printf("Invalid plot %s with variables %s and %s.\n", plot, 
						grapher->parm1, grapher->parm2);
		exit(1);
	}

	if ( grapher->parm1_index >= 0 || grapher->parm2_index >= 0 ) {
		printf("Found valid plot (%s, %s).\n", grapher->parm1, grapher->parm2);

		if ( ! strcmp(grapher->parm1, grapher->parm2) ) {
			printf("Warning: axes are plotting the same variable %s.\n", 
							grapher->parm1);
		}
	} else {
		printf("Fatal: Plot parameter %s is invalid.\n", plot);
		exit(1);
	}

	// Install the rest of the variables, if present.
	grapher->r0 = (double*)malloc(sizeof(double)*n_vars);
	char key[100];
	char *val;
	for (i = 0; i < n_vars; i++) {
		sprintf(key, ":%s", variable_order[i]);
		val = iniparser_getstring(options, key, "");
		if ( ! strcmp(val, "") ) {
			grapher->r0[i] = variable_defaults[i];
		} else {
			grapher->r0[i] = atof(val);
		}
	}
	grapher->r0_length = n_vars;

	// Do the limits for x, y, t.
	char *t_lims, *x_lims, *y_lims;
	char x_key[32], y_key[32];
	sprintf(x_key, ":%s", grapher->parm1);
	sprintf(y_key, ":%s", grapher->parm2);
	t_lims = iniparser_getstring(options, ":t", "");
	x_lims = iniparser_getstring(options, x_key, "");
	y_lims = iniparser_getstring(options, y_key, "");

	if ( (! parse_limits(x_lims, &grapher->parm1_limits[0])) &&
		 (! parse_limits(y_lims, &grapher->parm2_limits[0])) &&
		 (! parse_limits(t_lims, &grapher->t_limits[0])) ) {
		printf("Found valid limits for %s, %s, t.\n", 
				grapher->parm1, grapher->parm2);
	} else {
		printf("Found invalid limits for either %s, %s, or t.", 
			 grapher->parm1, grapher->parm2);
		exit(1);
	}

	// Find the comment, if any.
	char *comment;
	comment = iniparser_getstring(options, ":comment", "");
	grapher->comment = malloc(sizeof(comment));
	strcpy(grapher->comment, comment);
	printf("Found comment '%s'.\n", grapher->comment);

	// Get the filename base from the input file.
	char *config_filename = iniparser_getstring(options, ":filename", "");
	char *name;
	name = strtok(config_filename, ".");
	grapher->name = malloc(sizeof(name));
	strcpy(grapher->name, name);
	printf("Filename '%s' will be used as the base for output.\n", 
						grapher->name);

	// Are we going to try to use the GPU?
	char *use_gpu = iniparser_getstring(options, ":use_gpu", "");
	if ( ! strcmp(use_gpu, "true") ) {
		printf("Will attempt to use the GPU for calculations.\n");
		grapher->use_gpu = 1;
	} else {
		printf("Will use the CPU for calculations.\n");
		grapher->use_gpu = 0;
	}
	
	// The options have been parsed, free the space.
	iniparser_freedict(options);

	// Finish preparing the grapher.
    printf("------------------------------------\n");
	printf("Finished installing settings. Time for a few sanity checks.\n");
    printf("Working with %s and %s.\n", grapher->parm1, grapher->parm2);
    print_limits("t", grapher->t_limits);
    print_limits(grapher->parm1, grapher->parm1_limits);
    print_limits(grapher->parm2, grapher->parm2_limits);

    grapher->height = pixels(grapher->parm1_limits);
    grapher->width = pixels(grapher->parm2_limits);

    if ( grapher->height <= 0 || grapher->width <= 0 ) {
        printf("Either the height or width is invalid: %d x %d.\n",
                grapher->height, grapher->width);
        exit(1);
    } else {
        printf("Dimensions: %d x %d.\n", grapher->width, grapher->height);
    }

    printf("Starting each run with:\n");
    for (i = 0; i < grapher->r0_length; i++) {
		printf("%s: ", variable_order[i]);
		if ( i == grapher->parm1_index || i == grapher->parm2_index ) {
			printf("*\n");
		} else {
			printf("%f\n", grapher->r0[i]);
		}
    }

	if ( grapher->use_gpu ) {
		printf("Will attempt to use the system's GPU for calculations.\n");
	}
    printf("(appropriate values will be used for the chosen variables)\n");
	return(0);
}
