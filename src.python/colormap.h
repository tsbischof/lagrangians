#ifndef COLORMAP_H
#define COLORMAP_H

#include <stdio.h> 

int get_min_max(FILE *raw_file, int height, int width, 
		double *min, double *max);
void do_pixel(int val, int max, int n_points, int **colormap, int *rgb);
int do_colormap(FILE *raw_file, FILE *ppm_file, double resolution, 
		double min, double max, double modulus, int height, int width,
		int n_points, int **colormap);
int raw_to_ppm(char *raw_filename, char *ppm_filename, 
		double resolution, double modulus, int height, int width, 
		int n_points, int **colormap);

#endif
