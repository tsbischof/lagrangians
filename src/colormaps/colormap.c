#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#define RGB_DEPTH 255

void usage(void) {
	fprintf(stderr, "colormap <raw file> <resolution> <height>x<width> <n> [<RGB point>..]\n");
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

void raw_to_ppm(char *raw_filename, double resolution, int height, int width,
		int n, int **colormap) {
	double raw_max_val = 0.0;
	int max_val;
	FILE *raw_file;
	int rgb[3];
	double raw_val;
	int val;
	int i, j;

	raw_file = fopen(raw_filename, "rb");
	// Get the maximum value.
	for (i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fread(&raw_val, sizeof(double), 1, raw_file);
			if ( raw_val > raw_max_val ) {
				raw_max_val = raw_val;
			}
		}
	}
	max_val = (int)floor(raw_max_val/resolution);
	fprintf(stderr, "Found maximum value %d.\n", max_val);

	fseek(raw_file, 0, SEEK_SET);
	
	fprintf(stdout, "P3\n%d %d\n%d\n", width, height, RGB_DEPTH);
	for ( i = 0; i < height; i++) {
		for (j = 0; j < width; j++) {
			fread(&raw_val, sizeof(double), 1, raw_file);
			val = (int)floor(raw_val/resolution);
			do_pixel(val, max_val, n, colormap, &rgb[0]);
			fprintf(stdout, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(stdout, "\n");
	}
	fclose(raw_file);
}

int main(int argc, char *argv[]) {
	char *raw_filename;
	char *dim_data; 
	int height, width;
	int n;
	int **colormap;
	int j;
	double resolution;

	FILE *raw_file;

	if ( argc < 5 ) {
		usage();
		exit(1);
	}

	// Get the source filename, and check that it is there.
	raw_filename = strdup(argv[1]);
	raw_file = fopen(raw_filename, "rb");
	if ( raw_file == NULL ) {
		fprintf(stderr, "Could not open %s for reading.\n", raw_filename);
		exit(1);
	}
	fclose(raw_file);

	resolution = atof(argv[2]);

	// Get the dimensions.
	dim_data = strdup(argv[3]);
	height = atoi(strtok(dim_data, "x"));
	dim_data = strtok(NULL, "x");
	width = atoi(dim_data);
	fprintf(stderr, "Found dimensions %dx%d.\n", height, width);
	if ( height <= 0 || width <= 0 ) {
		fprintf(stderr, 
			"Image must have size greater than 0 in either dimension.\n");
		exit(1);
	}

	n = atoi(argv[4]); // The number of points to map on.	
	if ( n == 0 /*|| n == NULL*/ ) {
		fprintf(stderr, "Invalid number of points: %s.\n", argv[4]);
		exit(1);
	}
	
	if ( argc != (5 + 3*n) ) {
		fprintf(stderr, 
			"Incorrect number of arguments. Expected %d, got %d.\n", 5+3*n,
			argc);
		exit(1);
	}	

	colormap = (int **)malloc(sizeof(int)*n);
	for (j = 0; j < n; j++) {
		colormap[j] = (int *)malloc(sizeof(int)*3);
	}

	for (j = 5; j < 5+3*n; j++) {
		colormap[(j-5)/3][(j-5) % 3] = atoi(argv[j]);
		//fprintf(stderr, "(%d, %d): %d\n", (j-5)/3, (j-5)%3, atoi(argv[j]));
	}

	// Now we have the parameters, time to play with the data.
	raw_to_ppm(raw_filename, resolution, height, width, n, &colormap[0]);

	return(0);
}
