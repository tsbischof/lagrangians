#include <stdio.h>
#include <math.h>

#include "grapher.h"
#include "image_funcs.h"
#define RGB_SCALE 255 // maximum channel value

void image_to_ppm(Grapher *grapher, char *filename) {
	printf("--------------------------\n");
	printf("Writing image to ppm file: %s.\n", filename);

	FILE *output;
	output = fopen(filename, "w");
	fprintf(output, "P3\n# %s\n", grapher->comment);
	fprintf(output, "%d %d\n", grapher->width, grapher->height);
	fprintf(output, "%d\n", RGB_SCALE);

	printf("Getting maximum pixel value.\n");

	int i, j;
	double max_pixel = 0;
	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			if (max_pixel < grapher->image[i][j]) {
				max_pixel = grapher->image[i][j];
			}
		}
	}
	printf("Found maximum pixel value of %f.\n", max_pixel);

	int rgb[3];

	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			choose_RGB(grapher->image[i][j], max_pixel, &rgb[0]);
			fprintf(output, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
	printf("Finished writing ppm file %s.\n", filename);
}

void image_to_raw(Grapher *grapher, char *filename) {
	printf("---------------------------------\n");
	printf("Writing image to raw file: %s.\n", filename);

	FILE *output;
	output = fopen(filename, "w");
	fprintf(output, "%d %d\n", grapher->width, grapher->height);

	int i,j;

	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			fprintf(output, "%f ", grapher->image[i][j]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
	printf("Finished writing raw file %s.\n", filename);
}

void choose_RGB(double pixel, double max_pixel, int *rgb) {
	/* Choose the color based on the value of the pixel, relative
	 * to the maximum value.
	 */
	int val, i;

	val = (int)ceil((pixel/max_pixel)*((RGB_SCALE*3)+2));

	for (i = 0; i < 3; i++) {
		if (val < 0) {
			rgb[i] = 0;
		} else if (val > (RGB_SCALE)) {
			rgb[i] = RGB_SCALE;
		} else {
			rgb[i] = val;
		}
		val -= (RGB_SCALE+1);
	}
}
