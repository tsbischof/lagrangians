#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "grapher.h"
#include "image_funcs.h"

void print_limits(char *name, double *limits) {
	printf("%s: [%f, %f, %f]\n", name, limits[0], limits[1], limits[2]);
}

void do_image(Grapher *grapher) {
	/* Create the image. Allocate the pixel array and populate it using the
	 * integrator and rule.
	 */
	printf("------------------------------------\nPreparing to build image.\n");
    printf("Working with %s and %s.\n", grapher->parm1, grapher->parm2);
    print_limits("t", grapher->t_limits);
    print_limits(grapher->parm1, grapher->parm1_limits);
    print_limits(grapher->parm2, grapher->parm2_limits);

    grapher->height = pixels(grapher->parm1_limits);
    grapher->width = pixels(grapher->parm2_limits);

    if ( grapher->height <= 0 || grapher->width <= 0 ) {
        printf("Either the height or width is invalid: %d x %d.\n",
                grapher->height, grapher->width);
        exit(1);
    } else {
		printf("Dimensions: %d x %d.\n", grapher->width, grapher->height);
	}

	printf("Starting each run with r = (");
	int a;
	for (a = 0; a < grapher->r0_length; a++) {
		printf("%f", grapher->r0[a]);
		if ( a+1 != grapher->r0_length ) {
			printf(",");
		}
	}
	printf(")\n");
	printf("(appropriate values will be used for the chosen variables)\n");

	int i,j;
	grapher->image = malloc(grapher->height*sizeof(double));
	if ( grapher->image == NULL ) {
		printf("Insufficient memory to create the image rows.\n");
		exit(1);
	}
	for (a = 0; a < grapher->height; a++) {	
		grapher->image[a] = malloc(grapher->width*sizeof(double));
		if (grapher->image[a] == NULL) {
			printf("Insufficient memory to create row %d.\n", a);
			exit(1);
		}
	}
	printf("Successfully allocated image.\n");

	long int k = 0;
	long int total_pixels = grapher->height*grapher->width;

	double max_t = grapher->t_limits[2];
	double dt = grapher->t_limits[1];

	double t;
	double r[grapher->r0_length];
	double r0[grapher->r0_length];
	int done;
	
	printf("--------------------------------\nStarting the image run.\n");
//	omp_lock_t rule_lock;
//	omp_init_lock(&rule_lock);

#pragma omp parallel for private(t, done) firstprivate(r, r0) schedule(dynamic, 1000)
	for ( i = 0; i < grapher->height; i++) {
		for ( j = 0; j < grapher->width; j++) {
			if ( ++k % 100000 == 0 ) {
				printf("On pixel %ld of %ld.\n", k, total_pixels);;
			}

			t = grapher->t_limits[0];
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

			done = 0;

			while (t <= max_t && ! done) {
//				omp_set_lock(&rule_lock);
				grapher->integrate(&r[0], dt);
				if ( grapher->rule(&r[0], &r0[0]) ) {
					done = 1;
				} else {
					t += dt;
				}
//				omp_unset_lock(&rule_lock);
			}

			grapher->image[i][j] = t;
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
	char *name = malloc(sizeof(grapher->name)+4);
	sprintf(name, "%s.raw", grapher->name);
	image_to_raw(grapher, name);
}


void to_ppm(Grapher *grapher) {
	char *name = malloc(sizeof(grapher->name)+4);
	sprintf(name, "%s.ppm", grapher->name);
	image_to_ppm(grapher, name);
}
