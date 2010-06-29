#ifndef GRAPHER_H_
#define GRAPHER_H_

#ifdef USE_GPU
#include <brook/Stream.h>
#endif

typedef struct Grapher {
	char *name;
	char *comment;
	void (*integrate)(double *, double);
	double (*rule)(double *, double *, double, double *, int);
	int (*validate)(double *);
	double **image;

	char *parm1;
	int parm1_index;
	double parm1_limits[3];
	int width;

	char *parm2;
	int parm2_index;
	double parm2_limits[3];
	int height;

	int i;
	int j;

	double t_limits[3];

	double *r0; /* holds the default values for the starting vector.
				  * Some will be overridden for parm1 and parm2.
				  */
	int r0_length;

	int use_gpu;
#ifdef USE_GPU
	void (*gpu_kernel)(::brook::Stream indicesStream, 
						::brook::Stream resultsStream);
#endif

	int extend_time;
	double max_pixel;
} Grapher;

typedef struct Functions {
	char *integrate_names[100];
	void (*integrate_funcs[100])(double *, double);
	
	char *rule_names[100];
	double (*rule_funcs[100])(double *, double *, double, double *, int);

	int validate;
	char *validate_names[100];
	int (*validate_funcs[100])(double *);
} Functions; 

void print_limits(char *name, double *limits);
void do_image(Grapher *grapher);
void do_pixel(double *result, Grapher *grapher, 
				double *r, double *r0, int i, int j);
int pixels(double *limits);
void to_raw(Grapher *grapher);
void to_ppm(Grapher *grapher);
void grapher_free(Grapher *grapher);

#endif
