#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include <omp.h>
#include "grapher.h"
#include "image_funcs.h"

void print_limits(char *name, double *limits) {
	printf("%s: [%f, %f, %f]\n", name, limits[0], limits[1], limits[2]);
}

void do_image(Grapher *grapher) {
	/* Create the image. Allocate the pixel array and populate it using the
	 * integrator and rule.
	 */

	int i,j;
	grapher->image = malloc(grapher->height*sizeof(double));
	if ( grapher->image == NULL ) {
		printf("Insufficient memory to create the image rows.\n");
		exit(1);
	}
	for (i = 0; i < grapher->height; i++) {	
		grapher->image[i] = malloc(grapher->width*sizeof(double));
		if (grapher->image[i] == NULL) {
			printf("Insufficient memory to create row %d.\n", i);
			exit(1);
		}
	}
	printf("Successfully allocated image.\n");

	long int k = 0;
	long int total_pixels = grapher->height*grapher->width;

	double r[grapher->r0_length];
	double r0[grapher->r0_length]; 
	
	printf("--------------------------------\nStarting the image run.\n");

//#pragma omp parallel for firstprivate(r, r0) schedule(dynamic)
	for ( i = 0; i < grapher->height; i++) {
		for ( j = 0; j < grapher->width; j++) {
			if ( ++k % 100000 == 0 ) {
				printf("On pixel %ld of %ld.\n", k, total_pixels);;
			}

			grapher->image[i][j] = do_pixel(grapher, &r[0], &r0[0], i, j);
		}
	}

	printf("Building image complete.\n");
}

double do_pixel(Grapher *grapher, double *r, double *r0, int i, int j) {
	double t = grapher->t_limits[0];
	int m;
	for (m = 0; m < grapher->r0_length; m++) {
		if ( m == grapher->parm1_index ) {
			r0[m] = grapher->parm1_limits[0]
					 + grapher->parm1_limits[1]*i;
			r[m] = r0[m];
		} else if ( m == grapher->parm2_index ) {
			r0[m] = grapher->parm2_limits[0]
					+ grapher->parm2_limits[1]*j;
			r[m] = r0[m];
		} else {
			r[m] = 0;
		}
	}

	int done = 0;

	if ( grapher->validate(&r[0]) ) {
		while (t < grapher->t_limits[2] && ! done) {
			grapher->integrate(&r[0], grapher->t_limits[1]);
			if ( grapher->rule(&r[0], &r0[0]) ) {
				done = 1;
			} else {
				t += grapher->t_limits[1];
			}
		}

		if ( t > grapher->t_limits[2] ) {
			return(grapher->t_limits[2]);
		} else {
			return(t);
		}
	} else {
		return(grapher->t_limits[2]);
	}
}

int pixels(double *limits) {
	/* Tells how many pixels are required to span the limits with appropriate
	 * increments. 
	 */
	return((int)ceil((limits[2]-limits[0])/limits[1]) + 1);
}

void to_raw(Grapher *grapher) {
	char *name = malloc(sizeof(grapher->name)+4);
	sprintf(name, "%s.raw", grapher->name);
	image_to_raw(grapher, name);
}


void to_ppm(Grapher *grapher) {
	char *name = malloc(sizeof(grapher->name)+4);
	sprintf(name, "%s.ppm", grapher->name);
	image_to_ppm(grapher, name);
}
