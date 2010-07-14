#include "image_funcs.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void grapher_to_ppm(Grapher *grapher) {
	char *filename= (char*)malloc(strlen(grapher->name)+1+4);
	sprintf(filename, "%s.ppm", grapher->name);
	printf("--------------------------\n");
	printf("Writing image to ppm file: %s.\n", filename);

	FILE *output;
	output = fopen(filename, "w");
	fprintf(output, "P3\n# %s\n", grapher->comment);
	fprintf(output, "%d %d\n", grapher->width, grapher->height);

	printf("Getting maximum pixel value.\n");

	int i, j;
	double max_pixel = get_max_pixel(grapher);
	int rgb_scale = (int)floor(max_pixel/grapher->t_limits[1]);
	printf("Found maximum pixel value of %F.\n", max_pixel);
	fprintf(output, "%d\n", rgb_scale);

	int rgb[3];

	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			choose_RGB(grapher->image[i][j], max_pixel, rgb_scale, &rgb[0]);
			fprintf(output, "%d %d %d  ", rgb[0], rgb[1], rgb[2]);
		}
		fprintf(output, "\n");
	}
	fclose(output);
	printf("Finished writing ppm file %s.\n", filename);
	free(filename);
}

void grapher_to_raw(Grapher *grapher) {
	printf("---------------------------------\n");
	printf("Writing image to raw file: %s.\n", grapher->raw_filename);

	FILE *output;
	output = fopen(grapher->raw_filename, "wb");
	int i,j;

	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			fwrite(&grapher->image[i][j], sizeof(double), 1, output);
		}
	}
	fclose(output);
	printf("Finished writing raw file %s.\n", grapher->raw_filename);
}

void choose_RGB(double pixel, double max_pixel, int rgb_scale, int *rgb) {
	/* Choose the color based on the value of the pixel, relative
	 * to the maximum value.
	 */
	int val, i;

	val = (int)ceil((pixel/max_pixel)*((rgb_scale*3)+2));

	for (i = 0; i < 3; i++) {
		if (val < 0) {
			rgb[i] = 0;
		} else if (val > (rgb_scale)) {
			rgb[i] = rgb_scale;
		} else {
			rgb[i] = val;
		}
		val -= (rgb_scale+1);
	}
}

void raw_to_grapher(Grapher *grapher) {
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
		exit(1);
	}
	
	for (i = 0; i < grapher->height; i++) {
		for (j = 0; j < grapher->width; j++) {
			fread(&grapher->image[i][j], sizeof(double), 1, raw_file);
			if ( grapher->image[i][j] > grapher->max_pixel ) {
				grapher->max_pixel = grapher->image[i][j];
			}
		}
	}
	fclose(raw_file);
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

void restart_to_grapher(Grapher *grapher) {
	FILE *restart_file;
	restart_file = fopen(grapher->restart_filename, "rb");

	printf("--------------------------------------------------------------\n");
	printf("Reading in restart data from %s.\n", grapher->restart_filename);

	if ( restart_file == NULL ) {
		printf("Error: could not read restart file %s.\n", 
					grapher->restart_filename);
		exit(1);
	}
	
	int i, j;
	for (i = 0; i < grapher->height; i++) {
		fread(&grapher->finished_rows[i], sizeof(int), 1, restart_file);
/*		if ( grapher->finished_rows[i] ) {
			printf("Row %d is already finished.\n", i);
		} else {
			printf("Row %d remains to be finished.\n", i);
		} */
	}

	for (i = 0; i < grapher->height; i++) {
		if ( grapher->finished_rows[i] ) {
			for (j = 0; j < grapher->width; j++) {
				fread(&grapher->image[i][j], sizeof(double), 1, restart_file);
			}
		} else {
			fseek(restart_file, sizeof(double)*grapher->width, SEEK_CUR);
		}
	}
	fclose(restart_file);
	printf("--------------------------------------------------------------\n");
} 

void write_restart_row(Grapher *grapher, int row) {
    FILE *restart_file;
    restart_file = fopen(grapher->restart_filename, "r+b");

    // First, write the row data to file.
    fseek(restart_file, grapher->height*sizeof(int)
					+ row*grapher->width*sizeof(double), SEEK_SET);
	int j;
	for (j = 0; j < grapher->width; j++) {
		fwrite(&grapher->image[row][j], sizeof(double), 1, restart_file);
	}
   
    // Next, record that we have finished writing the data.
    fseek(restart_file, row*sizeof(int), SEEK_SET);
    int finished[1] = {1};
    fwrite(finished, sizeof(int), 1, restart_file);
    
	fclose(restart_file);
}

void allocate_restart_file(Grapher *grapher) {
	FILE *restart_file;
	restart_file = fopen(grapher->restart_filename, "r");
	if ( restart_file != NULL ) {
		printf("Data already exists in %s. \nThis run will overwrite the data, so move it out of the way or use:\n\trestart = true\n", grapher->restart_filename);
		fclose(restart_file);
		exit(1);
	}

	restart_file = fopen(grapher->restart_filename, "wb");

	if ( restart_file == NULL ) {
		printf("Error: could not open restart file %s for writing.\n",
					grapher->restart_filename);
		exit(1);
	} else {
		printf("Allocating %s on disk.\n", grapher->restart_filename);
	}

	int i;
	int unfinished[] = {0};
	for (i = 0; i < grapher->height; i++) {
		fwrite(unfinished, sizeof(int), 1, restart_file);
	}

	double blank_row[grapher->width];
	for (i = 0; i < grapher->width; i++) {
		blank_row[i] = 0;
	}
	for (i = 0; i < grapher->height; i++) {
		fwrite(blank_row, sizeof(double), grapher->width, restart_file);
	}

	printf("Successfully allocated %s on disk.\n", grapher->restart_filename);
	fclose(restart_file);
}

void restart_to_raw(Grapher *grapher) {
	FILE *restart_file, *raw_file;
	restart_file = fopen(grapher->restart_filename, "rb");
	raw_file = fopen(grapher->raw_filename, "wb");

	printf("-------------------------------------\n");
    printf("Converting temporary data from %s to raw data in %s.\n",
           grapher->restart_filename, grapher->raw_filename);


	if ( restart_file == NULL ) {
		printf("Error: could not open %s for reading.\n",
				grapher->restart_filename);
	} else if ( raw_file == NULL ) {
		printf("Error: could not open %s for writing.\n",
				grapher->raw_filename);
	} else {
		fseek(restart_file, sizeof(int)*grapher->height, SEEK_SET);
		int i, j;
		double value;
		for (i = 0; i < grapher->height; i++) {
			for (j = 0; j < grapher->width; j++) {
				fread(&value, sizeof(double), 1, restart_file);
				fwrite(&value, sizeof(double), 1, raw_file);
			}
		}

		printf("Finished writing data to %s.\n", grapher->raw_filename);
	}
}
