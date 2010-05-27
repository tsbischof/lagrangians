#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "grapher.h"

void print_limits(char *name, double *limits) {
	printf("%s: [%f, %f, %f]\n", name, limits[0], limits[1], limits[2]);
}

void do_image(Grapher *grapher) {
	/* Create the image. Allocate the pixel array and populate it using the
	 * integrator and rule.
	 */
	printf("---------------------------------------------\nBuilding image.\n");
    printf("Working with %s and %s.\n", grapher->parm1, grapher->parm2);
    print_limits("t", grapher->t_limits);
    print_limits(grapher->parm1, grapher->parm1_limits);
    print_limits(grapher->parm2, grapher->parm2_limits);
    printf("Dimensions: %d x %d.\n", grapher->width, grapher->height);

    grapher->height = pixels(grapher->parm1_limits);
    grapher->width = pixels(grapher->parm2_limits);

    if ( grapher->height <= 0 || grapher->width <= 0 ) {
        printf("Either the height or width is invalid: %d x %d.\n",
                grapher->height, grapher->width);
        exit(1);
    }

	printf("Starting each run with r = (");
	int a = 0;
	while ( a < grapher->r0_length ) {
		printf("%f", grapher->r0[a++]);
		if ( a != grapher->r0_length ) {
			printf(",");
		}
	}
	printf("\n");

	int i,j;
	double image[grapher->height][grapher->width];
	grapher->image = &image[0];

	long int k = 0;
	long int total_pixels = grapher->height*grapher->width;

	double max_t = grapher->t_limits[2];
	double dt = grapher->t_limits[1];

	double t;
	double r[grapher->r0_length];
	double *r0 = grapher->r0;

#pragma omp parallel for private(r, r0, t)
	for ( i = 0; i < grapher->height; i++) {
		for ( j = 0; j < grapher->width; j++) {
			if ( ++k % 1000 == 0 ) {
				printf("On pixel %l of %l.\n", k, total_pixels);;
			}

			t = grapher->t_limits[0];
			r0[grapher->parm1_index] = 
               grapher->parm1_limits[0] + grapher->parm1_limits[1]*i;
			r0[grapher->parm2_index] = 
               grapher->parm2_limits[0] + grapher->parm2_limits[1]*i;

			while (t <= max_t) {
				grapher->integrate(&r[0], dt);
				if ( grapher->rule(&r[0], &r0[0]) ) {
					break;
				} else {
					t += dt;
				}
			}

			image[i][j] = t;
		}
	}

	printf("Building image complete.\n");
}

int pixels(double *limits) {
	/* Tells how many pixels are required to span the limits with appropriate
	 * increments. 
	 */
	return((int)ceil((limits[2]-limits[0])/limits[1]) + 1);
}

void to_raw(Grapher *grapher) {
	char *name_base = grapher->name;
	char *name;
	sprintf(name, "%s.raw", name_base);
	image_to_raw(grapher->image, name);
}


void to_ppm(Grapher *grapher) {
	char *name_base = grapher->name;
	char *name;
	sprintf(name, "%s.ppm", name_base);
	image_to_raw(grapher->image, name);
}
