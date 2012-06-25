#ifndef COLORMAP_H
#define COLORMAP_H

#include <stdio.h> 

typedef struct {
	int n_points;
	int **points;
} colormap_t;

void usage();

colormap_t *allocate_colormap(int n_points);
void free_colormap(colormap_t **colormap);

int get_min_max(FILE *raw_file, int height, int width, 
		double *min, double *max);
void do_pixel(int val, int max, colormap_t *colormap, int *rgb);
int do_colormap(FILE *raw_file, int height, int width,
		FILE *ppm_file, 
		double resolution, double modulus,
		colormap_t *colormap); 

#endif
