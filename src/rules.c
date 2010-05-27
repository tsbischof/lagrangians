#include "rules.h"

const double PI = 3.14159265;

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

int first_flip(double *r, double *r0) {
	return ( (r[1] < PI*-2) || (r[1] > PI*2) );
}
