#include <stdio.h>

#include "parse_utils.h"
#include "installed.h"

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

	if ( (config_file = fopen(config_filename, "r")) != NULL ) {
		printf("Input file %s found. Parsing options.\n", config_filename);
	} else {
		printf("Input file %s could not be read.\n", config_filename);
		return(1);
	}

	char *integrator;
	fscanf(config_file, "integrator = %s\n", integrator);
	integrator = remove_whitespace(integrator);
	close(config_file);

	int (*dispatcher)(char *);

	if ( ! get_dispatcher(integrator, dispatcher) ) {
		printf("Integrator %s is installed.\n", integrator);
		return(dispatcher(config_filename));
	} else {
		printf("Integrator %s is not installed.\n", integrator);
		return(1);
	}
}
