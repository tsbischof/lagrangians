#ifndef COLORMAP_H
#define COLORMAP_H

#include <stdio.h> 

void do_pixel(int val, int max, int n_points, int **colormap, int *rgb);
int raw_to_ppm(FILE *raw_file, FILE *ppm_file, double resolution, 
		int height, int width, int n_points, int **colormap);
int raw_to_ppm_filenames(char *raw_filename, char *ppm_filename, 
		double resolution, int height, int width, int n_points, int **colormap);

#endif
