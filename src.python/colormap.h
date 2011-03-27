#ifndef COLORMAP_H
#define COLORMAP_H

#include <stdio.h> 

void do_pixel(int val, int max, int n_points, int **colormap, int *rgb);
void raw_to_ppm(FILE *raw_file, double resolution, 
			int n_points, int **colormap);

#endif
