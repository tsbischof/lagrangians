#include "rules.h"
#include <stdio.h>
#include <math.h>

int sign(double x) {
	if ( x >= 0 ) {
		return(1);
	}	else {
		return(-1);
	}
}

double first_turnaround(double *r, double *r0, double t, double *values, 
		int done) {
//	printf("%lf: %lf, %lf\n", t, r[1], r0[1]);
	if ( ! done ) {
		return( (r[1] == 0) || (sign(r[1]) == -1*sign(r0[1])) );
	} else {
		return(t);
	}
}

int validate_all(double *r) {
	return(1);
}

double speed(double *r, double *r0, double t, double *values, int done) {
	if ( ! done ) {	
		return( r[1] > 5 );
	} else {
		return(t);
	}
}

double flipped(double val, double t, int done) {
	if ( ! done ) {
		return( (val > 2*M_PI) || (val < -2*M_PI) || (! isfinite(val)) );
	} else {
		return(t);
	}
}
