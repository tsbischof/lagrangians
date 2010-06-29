#include "grapher.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include "image_funcs.h"
#include "time.h"

#ifdef USE_GPU
#include <brook/Stream.h>
#endif


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

	long int k = 0;
	long int total_pixels = grapher->height*grapher->width;

	double r[grapher->r0_length];
	double r0[grapher->r0_length]; 
	
	printf("--------------------------------\nStarting the image run.\n");

    int print_every = grapher->width*grapher->height / 1000;
    if ( print_every == 0 ) {
        print_every = 1;
    }

    time_t rawtime;
    struct tm * timeinfo;
    char fmttime[100];

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
		/* Here, we create a stream of inidices which represent the pixel 
		 * position. These are fed into the kernel, along with the appropriate
		 * system, integrator, and rule type, and output is a stream of doubles
		 * which represents the values for that row.
		 * 
		 * It may be possible to run this with OpenMP as well, with each thread
		 * running a separate row.
		 *
		 * This probably will work if the GPU routines are kept in separted .cpp
		 * files, and are accessed only through C-safe routines. Basically,
		 * pass the grapher to a helper function which then calls the corrent 
		 * kernel, handles errors, etc. This means less time worrying about 
		 * C++ conventions.
		 */
		double indices[grapher->width][2];
		::brook::Stream<int> indicesStream(2, grapher->width);
		::brook::Stream<double> resultStream(1, grapher->width);

		for (i = 0; i < grapher->width; i++) {
			indices[i][0] = i;
			indices[i][1] = 0;
		}
		
		for (i = 0; i < grapher->height; i++) {
			time(&rawtime);
            timeinfo = localtime(&rawtime);
            strftime(fmttime, 100, "%Y.%m.%d %H:%M:%S", timeinfo);
            printf("%s: On row %d of %d (%.1f%%).\n", fmttime, i,
					grapher->height, i/(float)grapher->height*100);

			indicesStream.read(indices);
			grapher->gpu_kernel(indicesStream, resultStream);
			resultStream.write(grapher->image[i]);
			for (j = 0; j < grapher->width; j++) {
				indices[j][1]++;
			}
		}
#else
		printf("Not compiled with GPU support. Sorry.\n");
#endif
	} else {
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
