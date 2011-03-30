#include <stdlib.h>
#include <math.h>
#include "colormap.h"

#define RGB_DEPTH 255

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

int do_colormap(FILE *raw_file, FILE *ppm_file, double resolution, 
		double min, double max, double modulus, int height, int width, 
		int n_points, int **colormap) {
	int max_val;
	int min_val;
	int rgb[3];
	double raw_val;
	int val;
	int i, j;

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
			do_pixel(val-min_val, max_val-min_val, n_points, colormap, &rgb[0]);
			fprintf(ppm_file, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(ppm_file, "\n");
	}

	return(0);
}

int raw_to_ppm(char *raw_filename, char *ppm_filename, 
		double resolution, double modulus, int height, int width, 
		int n_points, int **colormap) {
	double min, max;
	int status;

    FILE *raw_file, *ppm_file;
	raw_file = fopen(raw_filename, "rb");
	ppm_file = fopen(ppm_filename, "wb");
	if ( raw_file == NULL ) {
		fprintf(stderr, "Could not open %s.\n", raw_filename);
		return(-1);
	}
	if ( ppm_file == NULL ) {
		fprintf(stderr, "Could not open %s.\n", ppm_filename);
		return(-1);
	}

	if ( modulus == 0 ) {
		if ( get_min_max(raw_file, height, width, &min, &max) ) {
			return(-1);
		}
	} else {
		min = 0;
		max = modulus;
	}

	status =  do_colormap(raw_file, ppm_file, resolution, min, max, modulus,
						  height, width, n_points, &colormap[0]);

	if ( status ) {
		fprintf(stderr, "Conversion of %s to %s failed.\n", 
				raw_filename, ppm_filename);
	}
	fclose(raw_file);
	fclose(ppm_file);

	return(status);
}
