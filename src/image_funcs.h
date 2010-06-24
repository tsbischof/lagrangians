#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "grapher.h"

void image_to_ppm(Grapher *grapher, char *filename);
void image_to_raw(Grapher *grapher, char *filename);
void choose_RGB(double pixel, double max_pixel, int *rgb);
int read_image(Grapher *grapher);
double get_max_pixel(Grapher *grapher);

#endif 
