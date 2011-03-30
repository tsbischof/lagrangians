#include <stdlib.h>
#include <math.h>
#include "colormap.h"

#define RGB_DEPTH 255

void do_pixel(int val, int max, int n, int **colormap, int *rgb) {
	int i;
	int segment;
	int remainder;
	int length;
	double increment;

	segment = val / (max / (n-1));

	for (i = 0; i < 3; i++) {
		if ( segment < (n-1) ) {
			rgb[i] = colormap[segment][i];
			length = colormap[segment+1][i] - colormap[segment][i];
			remainder = val % (max / (n-1));
			increment = floor((double)remainder/((double)max/(double)(n-1)) *
							  (double)length);
			rgb[i] += (int)increment;
		} else {
			rgb[i] = colormap[n-1][i];
		}
		rgb[i] = abs(rgb[i]);
	}
}

// Put in actual checks of values, to avoid segfaulting. Return status of some nature, to tell the user that shit has gone down.

int raw_to_ppm(FILE *raw_file, FILE *ppm_file, double resolution, 
		int height, int width, int n_points, int **colormap) {
	double raw_max_val = 0.0;
	int max_val;
	double raw_min_val = 0.0;
	int min_val;
	int rgb[3];
	double raw_val;
	int val;
	int i, j;

	// Get the maximum value.
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if ( ! fread(&raw_val, sizeof(double), 1, raw_file) ) {
				fprintf(stderr, "Failed while reading raw file.\n");
				return(-1);
			}
			if ( raw_val > raw_max_val ) {
				raw_max_val = raw_val;
			}
			if ( raw_val < raw_min_val ) {
				raw_min_val = raw_val;
			}
		}
	}
	max_val = (int)floor(raw_max_val/resolution);
	min_val = (int)floor(raw_min_val/resolution);

	if ( max_val == min_val ) {
		fprintf(stderr, "Cannot convert image with no depth.\n");
		return(-2);
	}

	fseek(raw_file, 0, SEEK_SET);

	fprintf(ppm_file, "P3\n%d %d\n%d\n", width, height, RGB_DEPTH);
	for ( i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			if ( ! fread(&raw_val, sizeof(double), 1, raw_file) ) {
				fprintf(stderr, "Failed while reading raw file.\n");
				return(-1);
			}
			val = (int)floor(raw_val/resolution) - min_val;
			do_pixel(val-min_val, max_val-min_val, n_points, colormap, &rgb[0]);
			fprintf(ppm_file, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(ppm_file, "\n");
	}

	return(0);
}

int raw_to_ppm_filenames(char *raw_filename, char *ppm_filename, 
	double resolution, int height, int width, int n_points, int **colormap) {

	int status;

    FILE *raw_file, *ppm_file;
    raw_file = fopen(raw_filename, "rb");
    ppm_file = fopen(ppm_filename, "wb");
	if ( raw_file == NULL || ppm_file == NULL ) {
		fprintf(stderr, "Could not open %s for reading and %s for writing.\n",
				raw_filename, ppm_filename);
	}
	status =  raw_to_ppm(raw_file, ppm_file, resolution, height, width, 
						n_points, &colormap[0]);

	if ( status ) {
		fprintf(stderr, "Conversion of %s to %s failed.\n", 
				raw_filename, ppm_filename);
	}
	fclose(raw_file);
	fclose(ppm_file);

	return(status);
}

void phase_to_rgb(double val, int *rgb) {
    int section;
    int pair[2];
    int mag;
    rgb[0] = 0;
    rgb[1] = 0;
    rgb[2] = 0;

	double my_val = fmod(val, 2*M_PI);
	if ( my_val < 0 ) {
		my_val += 2*M_PI;
	}

    if ( my_val < 2*M_PI/3 ) {
        section = 0;
        pair[0] = 0;
        pair[1] = 1;
    } else if ( my_val < 4*M_PI/3 ) {
        section = 1;
        pair[0] = 1;
        pair[1] = 2;
    } else {
        section = 2;
        pair[0] = 2;
        pair[1] = 0;
    }

    mag = (int)floor(RGB_DEPTH*(3*my_val/(2*M_PI) - section));
    rgb[pair[0]] = mag;
    rgb[pair[1]] = RGB_DEPTH-mag;
}

int phase_to_ppm(char *raw_filename, char *ppm_filename, 
		int height, int width) {
	int i, j;
	FILE *raw_file;
	FILE *ppm_file;
	int rgb[3];
	double val;

	raw_file = fopen(raw_filename, "rb");
	ppm_file = fopen(ppm_filename, "w");
	if ( raw_file == NULL ) {
		fprintf(stderr, "Could not open %s for reading.\n", raw_filename);
		return(-1);
	} else if ( ppm_file == NULL ) {
		fprintf(stderr, "Could not open %s for writing.\n", ppm_filename);
		return(-1);
	} else {
		fprintf(ppm_file, "P3\n%d %d\n%d\n", width, height, RGB_DEPTH);
		for (i = 0; i < height; i++) {
			for (j = 0; j < width; j++) {
				if ( ! fread(&val, sizeof(double), 1, raw_file) ) {
					fprintf(stderr, "Failed while reading raw file.\n");
					return(-1);
				}
				phase_to_rgb(val, &rgb[0]);
				fprintf(ppm_file, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
			 }
			fprintf(ppm_file, "\n");
		}
	}

	fclose(ppm_file);
	fclose(raw_file);
	return(0);
}

