#include "rules.h"

double PI = 3.14159265;

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
	return ( (r[2] > 2*PI) || (r[2] < -2*PI) );
}

int validate_all(double *r) {
	return(1);
}

int speed(double *r, double *r0) {
	return( r[1] > 5 );
}

int first_flip_dangling_stick(double *r, double *r0) {
	return( (r[4] > 2*PI) || (r[4] < -2*PI) );
}
