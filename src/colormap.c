#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

#include "colormap.h"

#define VERBOSE 0

#define RGB_DEPTH 255

void usage() {
	fprintf(stderr,
"Usage: colormap [options] points\n"
"        -i, --file-in: Input file to process, which is a stream of double-\n"
"                       precision floats. By default, this is stdin.\n"
"       -o, --file-out: Output file to write the ppm file to. By default, \n"
"                       this is stdout.\n"
"          -W, --width: Width of the original raw image.\n"
"         -H, --height: Height of the original raw image.\n"
"-n, --colormap-length: Number of points in the colormap (RGB for each).\n"
"                       The default is black->red->yellow->white.\n"
"     -r, --resolution: Float resolution for pixels. By default, \n"
"                       this is 0.01.\n"
"        -m, --modulus: Modulus for pixel values. By default, this \n"
"                       is not used.\n"
"               points: The points in the colormap, as the RGB values\n"
"                       separated by spaces.\n"
"           -h, --help: Print this message.\n");      
}

colormap_t *allocate_colormap(int n_points) {
	colormap_t *colormap = NULL;

	int result = 0;
	int i;

	colormap = (colormap_t *)malloc(sizeof(colormap_t));

	if ( colormap == NULL ) {
		result = -1;
	} else {
		colormap->n_points = n_points;
		colormap->points = (int **)malloc(sizeof(int *)*n_points);

		result = (colormap->points == NULL);
		
		for ( i = 0; ! result && i < n_points; i++ ) {
			colormap->points[i] = (int *)malloc(sizeof(int)*3);
			result = (colormap->points[i] == NULL);
		}	
	}

	if ( result ) {
		free_colormap(&colormap);
		colormap = NULL;
	}

	return(colormap);
}

void free_colormap(colormap_t **colormap) {
	int i;

	if ( *colormap != NULL ) {
		for ( i = 0; i < (*colormap)->n_points; i++ ) {
			free( (*colormap)->points[i] ) ;
		}
		
		free(*colormap);
	}
}

int get_min_max(FILE *raw_file, int height, int width, 
		double *min, double *max) {
	double raw_val;
	int i;
	*min = 0;
	*max = 0;

	for (i = 0; i < height*width; i++) {
		if ( ! fread(&raw_val, sizeof(double), 1, raw_file) ) {
				fprintf(stderr, "Failed while reading raw file.\n");
				return(-1);
			}
		if ( raw_val > *max ) {
			*max = raw_val;
		}
		if ( raw_val < *min ) {
			*min = raw_val;
		}
	}
	fseek(raw_file, 0, SEEK_SET);
	return(0);
}

void do_pixel(int val, int max, colormap_t *colormap, int *rgb) {
	int i;
	int segment;
	int remainder;
	int length;
	double increment;

	segment = val / (max / (colormap->n_points-1));

	for (i = 0; i < 3; i++) {
		if ( segment < (colormap->n_points-1) ) {
			rgb[i] = colormap->points[segment][i];
			length = colormap->points[segment+1][i]
					 - colormap->points[segment][i];
			remainder = val % (max / (colormap->n_points-1));
			increment = floor((double)remainder/
							  ((double)max/(double)(colormap->n_points-1)) *
							  (double)length);
			rgb[i] += (int)increment;
		} else {
			rgb[i] = colormap->points[colormap->n_points-1][i];
		}
		rgb[i] = abs(rgb[i]);
	}
}

int do_colormap(FILE *raw_file, int height, int width,
		FILE *ppm_file, 
		double resolution, double modulus,  
		colormap_t *colormap) {
	double max;
	double min;
	int max_val;
	int min_val;
	int rgb[3];
	double raw_val;
	int val;
	int i, j;

	if  ( modulus == 0 ) {
		if ( get_min_max(raw_file, height, width, &min, &max) ) {
			return(-1);
		}
	} else {
		min = 0;
		max = modulus;
	}

	max_val = (int)floor(max/resolution);
	min_val = (int)floor(min/resolution);

	if ( max_val == min_val ) {
		fprintf(stderr, "Cannot convert image with no depth.\n");
		return(-2);
	}

	fprintf(ppm_file, "P3\n%d %d\n%d\n", width, height, RGB_DEPTH);
	for ( i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if ( ! fread(&raw_val, sizeof(double), 1, raw_file) ) {
				fprintf(stderr, "Failed while reading raw file.\n");
				return(-1);
			}
			if ( ! isfinite(raw_val) ) {
				raw_val = 0;
			}
/* If working with a modulus (phase, etc.), we want to be able to apply it 
 * safely and get out a number in [0, modulus). Otherwise, leave things be.
 */
			if ( modulus != 0) {
				raw_val = fmod(raw_val, modulus);
				if ( raw_val < 0 ) {
					raw_val += modulus;
				}
			}
			val = (int)floor(raw_val/resolution) - min_val;
			do_pixel(val-min_val, max_val-min_val, colormap, &rgb[0]);
			fprintf(ppm_file, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(ppm_file, "\n");
	}

	return(0);
}

int main(int argc, char *argv[]) {
	int c;
	int option_index = 0;

	FILE *in_stream = NULL;
	char *in_filename = NULL;
	FILE *out_stream = NULL;
	char *out_filename = NULL;

	int height = 0;
	int width = 0;
	int colormap_length = 0;
	colormap_t *colormap = NULL;

	double resolution = 0.01;
	double modulus = 0;

	int i, point_index, rgb_index; 
	int result = 0;

	static struct option long_options[] = {
		{"help", no_argument, 0, 'h'},
		{"file-in", required_argument, 0, 'i'},
		{"file-out", required_argument, 0, 'o'},
		{"width", required_argument, 0, 'W'},
		{"height", required_argument, 0, 'H'},
		{"colormap-length", required_argument, 0, 'n'},
		{"modulus", required_argument, 0, 'm'},
		{"resolution", required_argument, 0, 'r'},
		{0, 0, 0, 0}};

	while ( (c = getopt_long(argc, argv, "hi:o:W:H:n:m:r:", long_options,
				&option_index)) != -1 ) {
		switch (c) { 
			case 'h':
				usage();
				return(0);
			case 'i':
				in_filename = strdup(optarg);
				break;
			case 'o':
				out_filename = strdup(optarg);
				break;
			case 'W':
				width = strtol(optarg, NULL, 10);
				break;
			case 'H':
				height = strtol(optarg, NULL, 10);
				break;
			case 'n':
				colormap_length = strtol(optarg, NULL, 10);
				break;
			case 'm':
				modulus = strtof(optarg, NULL);
				break;
			case 'r':
				resolution = strtof(optarg, NULL);
				break;
			case '?':
			default:
				usage();
				return(-1);
		}
	}

	if ( in_filename == NULL ) {
		in_stream = stdin;
	} else {
		in_stream = fopen(in_filename, "rb");
		if ( in_stream == NULL ) {
			fprintf(stderr, "Could not open %s for reading.\n", in_filename);
			result = -1;
		}
	}

	if ( out_filename == NULL ) {
		out_stream = stdout;
	} else {
		out_stream = fopen(out_filename, "w");
		if ( out_stream == NULL ) {
			fprintf(stderr, "Could not open %s for writing.\n", out_filename);
			result = -1;
		}
	}

	if ( colormap_length >= 1 ) {
		colormap = allocate_colormap(colormap_length);
		if ( colormap == NULL ) {
			fprintf(stderr, "Could not allocate memory for the colormap.\n");
			result = -1;
		}
	} else {
		fprintf(stderr, "Length of colormap must be at least 1 "
				"(%d specified).\n", colormap_length);
		result = -1;
	}

	if ( width <= 0 || height <= 0 ) {
		fprintf(stderr, "Height and width must be at least 1 "
				"(%d, %d specified.\n", height, width);
		result = -1;
	}

	
	if ( argc - optind != 3*colormap_length ) {
		fprintf(stderr, "Number of values specified (%d) is not the same as "
				"the number found (%d).\n", 
				colormap_length*3, 
				argc-optind);
		result = -1;
	} else {
		point_index = 0;
		rgb_index = 0;
		for ( i = optind; ! result && i < argc; i++ ) {
			colormap->points[point_index][rgb_index] = 
					strtol(argv[i], NULL, 10);
			
			if ( abs(colormap->points[point_index][rgb_index] > RGB_DEPTH )) {
				fprintf(stderr,
						"RGB value out of range: %d (%d limit)\n",
						colormap->points[point_index][rgb_index],
						RGB_DEPTH);
				result = -1;
			}

			rgb_index++;
			if ( rgb_index == 3 ) {
				rgb_index = 0;
				point_index++;
			}
		}
	}

	if ( ! result ) {
		do_colormap(in_stream, height, width,
				out_stream,
				resolution, modulus,
				colormap);
	}
	
	/* Clean up memory. */
	printf("colormap\n");
	free_colormap(&colormap);
	printf("in\n");
	free(in_filename);
	printf("out\n");
	free(out_filename);
	
	if ( in_stream && in_stream != stdin ) {
		fclose(in_stream);
	} 
	if ( out_stream && out_stream != stdout ) {
		fclose(out_stream);
	}


	return(0);
}
