#include "includes.h"
#include "dangling_stick.h"

/* Simulates the motion of a massless stick with a mass on either end, attached
 * to a spring.
 */

double m1 = 1; // mass attached to the spring
double m2 = 1; // mass attached to the other end of the stick
double r0 = 1; // natural length of the spring
double k = 1; // spring constant
double l = 1; // length of the massless stick
double g = 9.8; 

enum { R, DR, THETA, DTHETA, PHI, DPHI };

void derivs_dangling_stick(double *r, double *drdt) {
	drdt[R] = r[DR];
	drdt[THETA] = r[DTHETA];
	drdt[PHI] = r[DPHI];

	drdt[DR] = 1.0/(2*m1*(m1+m2))  
			  * ( k*(2*m1+m2)*(r0-r[R]) 
				+ 2*m1*(m1+m2)*r[R]*pow(r[DTHETA],2)
				+ 2*g*m1*(m1+m2)*cos(r[THETA])
				+ 2*l*m1*m2*pow(r[DPHI],2)*cos(r[THETA]-r[PHI])
				- k*m2*(r0-r[R])*cos(2*(r[THETA]-r[PHI])));
	drdt[DTHETA] = 1.0/(2*m1*(m1+m2)*r[R]) 
				  * ( k*m2*(r0-r[R])*sin(2*(r[THETA]-r[PHI])) 
					- 2*g*m1*(m1+m2)*sin(r[THETA]) 
					- 4*m1*(m1+m2)*r[DR]*r[DTHETA]
					+ 2*l*m1*m2*pow(r[DPHI],2)*sin(r[THETA]-r[PHI]));
	drdt[DPHI] = k*(r0-r[R])*sin(r[THETA]-r[PHI])/(l*m1);
}

void integrate_dangling_stick(double *r, double dt) {
	runge_kutta_4(derivs_dangling_stick, r, dt, 6);
}

double T_dangling_stick(double *r) {
	return( 1.0/2*m1*( pow(r[R]*r[DTHETA]*cos(r[THETA])
						  +r[DR]*sin(r[THETA]),2) +
				pow(-r[DR]*cos(r[THETA])
					+r[R]*r[DTHETA]*sin(r[THETA]), 2)) +
			1.0/2*m2*( 
				pow(r[R]*r[DTHETA]*cos(r[THETA])
				   +r[DR]*sin(r[THETA])
				   +l*r[DPHI]*cos(r[PHI]), 2)
			  + pow(-r[DR]*cos(r[THETA])
					+r[R]*r[DTHETA]*sin(r[THETA])
					+l*r[DPHI]*sin(r[PHI]),2)));
}

double U_dangling_stick(double *r) {
	return(k/2*pow(r[R]-r0,2)
		- m2*g*(r[R]*cos(r[THETA])+l*cos(r[PHI]))
		- m1*g*r[R]*cos(r[THETA]) );
}

int energy_dangling_stick(double *r) {
	extern double PI;
	double r_min[6];
	r_min[R] = 0;
	r_min[DR] = PI;
	r_min[THETA] = 0;
	r_min[DTHETA] = 0;
	r_min[PHI] = 0;
	r_min[DPHI] = 0;

	return( U_dangling_stick(r) + T_dangling_stick(r) 
			> U_dangling_stick(&r_min[0]) );
} 

void do_dangling_stick(dictionary *options, Grapher *grapher) {
	char *valid_rules = {"first_flip"};
	char *variable_order[6] = {"r", "dr", "theta", "dtheta", "phi", "dphi"};
	setup_config(grapher, options, &variable_order[0], 6, &valid_rules);
}
