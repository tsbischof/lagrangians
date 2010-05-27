#include <stdio.h>

#include "parse_utils.h"
#include "installed.h"
#include "iniparser/iniparser.h"

int main(int argc, char *argv[]) {
	// To do: write a raw image every so often, as a backup. Allow restart from the dumped file.
	// To do: extend time limit by identifying whether the pixel is at the maximum value and running the appropriate starting conditions.
	// To do: have usage print a list of installed features

	if (argc != 2) {
		printf("Usage: lagrangians input_file.\n");
		return(1);
	}

	char *config_filename = argv[1];
	printf("Reading input from %s.\n", config_filename);
	FILE *config_file;
	
	config_file = fopen(config_filename, "r");
	if ( config_file ) {
		printf("Input file %s found. Parsing options.\n", config_filename);
		fclose(config_file);
	} else {
		printf("Input file %s could not be read.\n", config_filename);
		fclose(config_file);
		return(1);
	}

	dictionary *options;
	options = iniparser_load(config_filename);
	if ( options == NULL ) {
		printf("Could not parse file: %s\n", config_filename);
		return(1);
	}

	char *integrator;
	integrator = iniparser_getstring(options, ":integrator", "");
	if ( integrator == "" ) {
		printf("No 'integrator = ... ' line found.\n");
		return(1);
	}

	int (*dispatcher)(dictionary *);
	dispatcher = get_dispatcher(integrator);
	if ( dispatcher != NULL ) {
		printf("Integrator %s is available.\n", integrator);
		dispatcher(options);
	} else {
		printf("Integrator %s is not available.\n", integrator);
		return(1);
	}
	
	return(0);
}
