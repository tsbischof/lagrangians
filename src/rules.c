#include "rules.h"

int sign(double x) {
	if ( x >= 0 ) {
		return(1);
	}	else {
		return(-1);
	}
}

double first_turnaround(double *r, double *r0, double t, double *values, 
		int done) {
	if ( ! done ) {
		return( (r[1] == 0) || (sign(r[1]) == sign(r0[1])*-1) );
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
