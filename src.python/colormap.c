#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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

void raw_to_ppm(FILE *raw_file, FILE *ppm_file, 
		double resolution, int height, int width, int n, int **colormap) {
	double raw_max_val = 0.0;
	int max_val;
	double raw_min_val = 0.0;
	int min_val;
	int rgb[3];
	double raw_val;
	int val;
	int i, j;
	int rgb_depth;

	// Get the maximum value.
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fread(&raw_val, sizeof(double), 1, raw_file);
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
	rgb_depth = (max_val-min_val)/3;

	fseek(raw_file, 0, SEEK_SET);
	
	fprintf(ppm_file, "P3\n%d %d\n%d\n", width, height, rgb_depth);
	for ( i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fread(&raw_val, sizeof(double), 1, raw_file);
			val = (int)floor(raw_val/resolution) - min_val;
			do_pixel(val, max_val-min_val, n, colormap, &rgb[0]);
			fprintf(ppm_file, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(ppm_file, "\n");
	}
}
