#ifndef GRAPHER_H_
#define GRAPHER_H_

typedef struct Grapher {
	char *name;
	char *comment;
	void (*integrate)(double *, double);
	int (*rule)(double *, double *);
	double **image;

	char *parm1;
	int parm1_index;
	double *parm1_limits;
	int width;

	char *parm2;
	int parm2_index;
	double *parm2_limits;
	int height;

	double *t_limits;

	double *r0; /* holds the default values for the starting vector.
				  * Some will be overridden for parm1 and parm2.
				  */
	int r0_length;
} Grapher;

void do_image(Grapher *grapher);
int pixels(double *limits);
void to_raw(Grapher *grapher);
void to_raw(Grapher *grapher);

#endif
