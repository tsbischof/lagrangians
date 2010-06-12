#ifndef GRAPHER_H_
#define GRAPHER_H_

typedef struct Grapher {
	char *name;
	char *comment;
	void (*integrate)(double *, double);
	int (*rule)(double *, double *);
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

	double t_limits[3];

	double r0[100]; /* holds the default values for the starting vector.
				  * Some will be overridden for parm1 and parm2.
				  */
	int r0_length;
} Grapher;


void print_limits(char *name, double *limits);
void do_image(Grapher *grapher);
double do_pixel(Grapher *grapher, double *r, double *r0, int i, int j);
int pixels(double *limits);
void to_raw(Grapher *grapher);
void to_ppm(Grapher *grapher);

#endif
