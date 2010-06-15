#include "rules.h"

int sign(double x) {
	if ( x >= 0 ) {
		return(1);
	}	else {
		return(-1);
	}
}

int first_turnaround(double *r, double *r0) {
	return( (r[1] == 0) || (sign(r[1]) == sign(r0[1])*-1) );
}

int validate_all(double *r) {
	return(1);
}

int speed(double *r, double *r0) {
	return( r[1] > 5 );
}
