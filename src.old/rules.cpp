#include <vector>

#define PI 3.14159265

using namespace std;

int sign(double x) {
	if ( x >= 0 ) {
		return(1);
	}	else {
		return(-1);
	}
}

bool first_turnaround(vector<double> *r, vector<double> *r0) {
	if ( (*r)[1] == 0 or sign((*r)[1]) == sign((*r0)[1])*-1) {
		return(true);
	} else {
		return(false);
	}
}

bool first_flip(vector<double> *r, vector<double> *r0) {
	if ( (*r)[1] < PI*-2 or (*r)[1] > PI*2 ) {
		return(true);
	} else {
		return(false);
	}
}
