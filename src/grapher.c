#include "grapher.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>
#include <time.h>
#include "image_funcs.h"

#ifdef USE_GPU
#include "systems/gpu/gpu_brook.h"
//#include "systems/gpu/gpu_opencl.h"
#endif

enum { NOT_DONE, DONE };

void grapher_free(Grapher *grapher) {
	free(grapher->name);
	free(grapher->system);

	free(grapher->raw_filename);
	free(grapher->restart_filename); 
	free(&grapher->finished_rows[0]);
	free(grapher->comment);
	
    int i;
    for ( i = 0; i < grapher->height; i++) {
        free(&grapher->image[i][0]);
    }
	free(&grapher->image[0]);

	free(grapher->parm1);
	free(grapher->parm2);
    free(&grapher->r0[0]);

//	free(grapher->gpu_type);
}

void print_limits(char *name, double *limits) {
	printf("%s: [%f, %f, %f]\n", name, limits[0], limits[1], limits[2]);
}

void allocate_grapher_image(Grapher *grapher) {
	int i;
    grapher->image = (double **)malloc(grapher->height*sizeof(double *));
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
    printf("Successfully allocated image in memory.\n");
}
	

void do_image(Grapher *grapher) {
	/* Create the image. Allocate the pixel array and populate it using the
	 * integrator and rule.
	 */

	printf("--------------------------------\nStarting the image run.\n");

	if ( ! grapher->restart ) {
		allocate_restart_file(grapher);
	} else {
		restart_to_grapher(grapher);
	}

	if ( grapher->extend_time ) {
		raw_to_grapher(grapher);
	}

	if ( grapher->use_gpu ) {
#ifdef USE_GPU
		printf("Using the GPU.\n");
		allocate_grapher_image(grapher); // for memory
		gpu_brook(grapher);
#else
		printf("GPU support not enabled.\n");
		exit(1);
#endif
	} else {
		printf("Using the CPU.\n");

    	time_t rawtime;
    	struct tm * timeinfo;
    	char fmttime[100];

   		double r[grapher->r0_length];
    	double r0[grapher->r0_length];

    	int i, j;
    	long int k = 0;
    	long int total_pixels = grapher->height*grapher->width;

    	int print_every = grapher->width*grapher->height / 1000;
    	if ( print_every == 0 ) {
        	print_every = 1;
    	}

		for (i = 0; i < grapher->r0_length; i++) {
			r0[i] = grapher->r0[i];
		}
		
		double my_row[grapher->width];

#pragma omp parallel for private(i, j) firstprivate(r, r0, my_row) schedule(dynamic)
		for (i = 0; i < grapher->height; i++) {
			if ( grapher->finished_rows[i] ) {
				printf("Found finished row %d.\n", i);
				k += grapher->width;
			} else {
				for (j = 0; j < grapher->width; j++) {
					if ( k % print_every == 0 ) {
						rawtime = time(NULL);
						timeinfo = localtime(&rawtime);
						strftime(fmttime, 100, "%Y.%m.%d %H:%M:%S", timeinfo);
						printf("%s: On pixel %ld of %ld (%.1f%%).\n", 
							fmttime, k, total_pixels, 
							k/(double)total_pixels*100);
					}
					k++;
					do_pixel(&my_row[j], grapher, &r[0], &r0[0], i, j);
				}

				write_restart_row(grapher, &my_row[0], i, 1);
				grapher->finished_rows[i] = 1;
			}
		}

		if ( grapher->extend_time && (!grapher->restart) ) {
			for (i = 0; i < grapher->height; i++) {
				free(grapher->image[i]);
			}
			free(grapher->image);
		}
	}

	printf("Building image complete.\n");

	printf("------------------------------------\n");

	raw_to_grapher(grapher);

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

	r0[grapher->parm1_index] = grapher->r0[grapher->parm1_index]
							+ i * grapher->parm1_limits[1];
	
	r0[grapher->parm2_index] = grapher->r0[grapher->parm2_index]
							+ j * grapher->parm2_limits[1];
	int m;
    for (m = 0; m < grapher->r0_length; m++) {
            r[m] = r0[m];
    }

	double values[10];
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

