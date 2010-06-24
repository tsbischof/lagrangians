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
	double max_pixel = get_max_pixel(grapher);
	printf("Found maximum pixel value of %F.\n", max_pixel);

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
	output = fopen(filename, "wb");
	int i,j;

	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			fwrite(&grapher->image[i][j], 1, sizeof(double), output);
		}
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

int read_image(Grapher *grapher) {
/* Read in the file to the image stored in the grapher, and record the 
 * maximum pixel value for later use. This is useful for restarts and 
 * other manipulations of the image.
 */
	int i, j;
	grapher->max_pixel = 0;

	FILE *raw_file;
	char filename[100];
	sprintf(filename, "%s.raw", grapher->name);
	raw_file = fopen(filename, "r");
	if ( raw_file == NULL ) {
		printf("Fatal: Could not find starting file %s.\n", filename);
		return(1);
	}
	
	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			fread(&grapher->image[i][j], 1, sizeof(double), raw_file);
			if ( grapher->image[i][j] > grapher->max_pixel ) {
				grapher->max_pixel = grapher->image[i][j];
			}
		}
	}
	fclose(raw_file);
	return(0);
}

double get_max_pixel(Grapher *grapher) {
	int i,j;
	double result = 0.0;
	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			if (result < grapher->image[i][j]) {
				result = grapher->image[i][j];
			}
		}
	}
	return(result);
}
