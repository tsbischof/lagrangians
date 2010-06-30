#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "grapher.h"

void grapher_to_ppm(Grapher *grapher);
void grapher_to_raw(Grapher *grapher);
void choose_RGB(double pixel, double max_pixel, int *rgb);
void raw_to_grapher(Grapher *grapher);
double get_max_pixel(Grapher *grapher);
void restart_to_grapher(Grapher *grapher);
void allocate_restart_file(Grapher *grapher);
void write_restart_row(Grapher *grapher, int row);
void restart_to_raw(Grapher *grapher);

#endif 
