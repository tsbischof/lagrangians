#include "grapher.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "image_funcs.h"
#include "time.h"

//#include "systems/gpu/brook_base.h"

enum { NOT_DONE, DONE };

void grapher_free(Grapher *grapher) {
	free(&grapher->image[0][0]);
	free(&grapher->r0[0]);
}

void print_limits(char *name, double *limits) {
	printf("%s: [%f, %f, %f]\n", name, limits[0], limits[1], limits[2]);
}

void do_image(Grapher *grapher) {
	/* Create the image. Allocate the pixel array and populate it using the
	 * integrator and rule.
	 */

	int i,j;
	grapher->image = (double **)malloc(grapher->height*sizeof(double));
	if ( grapher->image == NULL ) {
		printf("Insufficient memory to create the image rows.\n");
		exit(1);
	}
	for (i = 0; i < grapher->height; i++) {	
		grapher->image[i] = (double*)malloc(grapher->width*sizeof(double));
		if (grapher->image[i] == NULL) {
			printf("Insufficient memory to create row %d.\n", i);
			exit(1);
		}
	}
	printf("Successfully allocated image.\n");

	double r[grapher->r0_length];
	double r0[grapher->r0_length]; 
	
	printf("--------------------------------\nStarting the image run.\n");

	if ( grapher->extend_time ) {
		if (!read_image(grapher)) {
			printf("Successfully read in the existing image.\n");
		} else {
			printf("Fatal: failed while reading the existing image %s.raw.\n",
					grapher->name);
			exit(1);
		}
	}

	if ( grapher->use_gpu ) {
#ifdef USE_GPU
		printf("Using the GPU.\n");
		grapher->gpu_kernel(grapher);
#else
		printf("GPU computation not supported. Sorry.\n");
		exit(1);
#endif
	} else {
	    long int k = 0;
		long int total_pixels = grapher->height*grapher->width;

	    int print_every = grapher->width*grapher->height / 1000;
	    if ( print_every == 0 ) {
        	print_every = 1;
    	}

    	time_t rawtime;
    	struct tm * timeinfo;
    	char fmttime[100];

		printf("Using the CPU.\n");

#pragma omp parallel for private(i, j) firstprivate(r, r0) schedule(dynamic)
		for ( i = 0; i < grapher->height; i++) {
			for ( j = 0; j < grapher->width; j++) {
				if ( k % print_every == 0 ) {
					time(&rawtime);
					timeinfo = localtime(&rawtime);
					strftime(fmttime, 100, "%Y.%m.%d %H:%M:%S", timeinfo);
					printf("%s: On pixel %ld of %ld (%.1f%%).\n", 
						fmttime, k, total_pixels, 
						k/(float)total_pixels*100);
				}
				k++;
	
				do_pixel(&grapher->image[i][j], grapher, &r[0], &r0[0], i, j);
			}
		}
	}

	printf("Building image complete.\n");
	grapher->max_pixel = get_max_pixel(grapher);
	if ( grapher->max_pixel == grapher->t_limits[2] ) {
		printf("Some pixels hit the maximum value of %f.\n", 
				grapher->t_limits[2]);
	} else {
		printf("Time limits exceeded the maximum pixel value.\n");
	}
}

void do_pixel(double *result, Grapher *grapher, 
			double *r, double *r0, int i, int j) {
	double t = grapher->t_limits[0];

	/* If we are extending time, we only need to run those pixels which are
 	 * currently at their limits. This assumes that the values increase with
 	 * time, but if needed this can be changed for certain rules.
 	 */
	if ( grapher->extend_time ) {
		if ( grapher->image[i][j] < grapher->max_pixel ) {
			return;
		}
	}

    int m;
    for (m = 0; m < grapher->r0_length; m++) {
        if ( m == grapher->parm1_index ) {
            r0[m] = grapher->parm1_limits[0]
                     + grapher->parm1_limits[1]*i;
        } else if ( m == grapher->parm2_index ) {
            r0[m] = grapher->parm2_limits[0]
                     + grapher->parm2_limits[1]*j;
        } else {
            r0[m] = grapher->r0[m];
        }
            r[m] = r0[m];
    }

	double values[100];
	if ( grapher->validate(&r[0]) ) {
		while (t < grapher->t_limits[2] ) {
			grapher->integrate(&r[0], grapher->t_limits[1]);
			if ( grapher->rule(&r[0], &r0[0], t, &values[0], NOT_DONE) ) {
				break;
			} else {
				t += grapher->t_limits[1];
			}
		}
	} else {
		t = grapher->t_limits[2];
	}

   if ( t > grapher->t_limits[2] ) {
        t = grapher->t_limits[2];
    }

	*result = grapher->rule(&r[0], &r0[0], t, &values[0], DONE);
}

int pixels(double *limits) {
	/* Tells how many pixels are required to span the limits with appropriate
	 * increments. 
	 */
	return((int)ceil((limits[2]-limits[0])/limits[1])+1);
}

void to_raw(Grapher *grapher) {
	char *name = (char*)malloc(sizeof(grapher->name)+4);
	sprintf(name, "%s.raw", grapher->name);
	image_to_raw(grapher, name);
}


void to_ppm(Grapher *grapher) {
	char *name = (char*)malloc(sizeof(grapher->name)+4);
	sprintf(name, "%s.ppm", grapher->name);
	image_to_ppm(grapher, name);
}
