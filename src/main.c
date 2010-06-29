#include <stdio.h>

#include "parse_utils.h"
#include <iniparser.h>
#include "installed.h"

int main(int argc, char *argv[]) {
	// To do: write a raw image every so often, as a backup. Allow restart from the dumped file.
	// To do: extend time limit by identifying whether the pixel is at the maximum value and running the appropriate starting conditions.
	// To do: have usage print a list of installed features

	if (argc != 2) {
		printf("Usage: lagrangians input_file.\n");
		printf("You invoked using: ");
		int i;
		for (i = 0; i < argc; i++) {
			printf("%s ", argv[i]);
		}
		printf("\n");
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

	char *system;
	system = iniparser_getstring(options, ":system", "");
	if ( ! strcmp(system, "") ) {
		printf("Fatal: No 'system = ... ' line found.\n");
		return(1);
	}

	dictionary_set(options, ":filename", config_filename);

	void (*dispatcher)(dictionary *, Grapher *);
	dispatcher = get_dispatcher(system);
	Grapher grapher;
	grapher.system = system;
	if ( dispatcher != NULL ) {
		printf("System %s is available.\n", system);
		dispatcher(options, &grapher);
		do_image(&grapher);
		to_raw(&grapher);
		to_ppm(&grapher);
		grapher_free(&grapher);
		return(0);
	} else {
		printf("Integrator %s is not available.\n", system);
		return(1);
	}
}
