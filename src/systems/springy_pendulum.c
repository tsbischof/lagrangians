#include "springy_pendulum.h"

/* Simulates the motion of a pendlum with a mass on a spring attached. The 
 * pendulum arm has no mass, but a mass is attached to the end.
 * r: length of the spring
 * phi1: angle of the pendulum, relative to vertical.
 * phi2: angle of the spring, relative to vertical.
 * m1: mass of the mass connected to the pendulum
 * m2: mass of the mass connected to the spring
 * k: spring constant for the spring
 * l: length of the stick
 * A `d' in front of the variable indicates a time derivative.
 */

enum { R, DR, PHI1, DPHI1, PHI2, DPHI2, M1, M2, R0, K, L, G };

void do_springy_pendulum(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "springy_pendulum";
	functions.integrate_funcs[0] = integrate_springy_pendulum;

	functions.rule_names[0] = "lower_flip";
	functions.rule_funcs[0] = lower_flip_springy_pendulum;

	functions.rule_names[1] = "upper_flip";
	functions.rule_funcs[1] = upper_flip_springy_pendulum;

    char *variable_order[12] = {"r", "dr", "phi1", "dphi1", "phi2", "dphi2",
								"m1", "m2", "r0", "k", "l", "g"};
	double variable_defaults[12] = {0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 9.8};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
				12, &functions);
}

void derivs_springy_pendulum(double *r, double *drdt) {
	drdt[DR] = -r[G]+(r[K]*r[R0])/r[M2]+r[G]*cos(r[PHI2])+r[L]*cos(r[PHI1]-r[PHI2])*r[DPHI1]*r[DPHI2]+r[R]*(-(r[K]/r[M2])+pow(r[DPHI2],2));
	drdt[DPHI1] = (r[L]*r[M2]*pow(cos(r[PHI1]-r[PHI2]),2)*r[DR]*r[DPHI1]-r[M2]*pow(r[R],2)*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2)-(r[R]*(2*r[G]*r[M1]*sin(r[PHI1])+r[G]*r[M2]*sin(r[PHI1])+r[G]*r[M2]*sin(r[PHI1]-2*r[PHI2])+r[L]*r[M2]*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI1],2)-2*r[M2]*cos(r[PHI1]-r[PHI2])*r[DR]*r[DPHI2]))/2.)/(r[L]*(r[M1]+r[M2]-r[M2]*pow(cos(r[PHI1]-r[PHI2]),2))*r[R]);
	drdt[DPHI2] = (-2*r[L]*(r[M1]+r[M2])*cos(r[PHI1]-r[PHI2])*r[DR]*r[DPHI1]+r[M2]*pow(r[R],2)*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI2],2)+r[R]*(2*r[G]*(r[M1]+r[M2])*cos(r[PHI1])*sin(r[PHI1]-r[PHI2])+2*r[L]*(r[M1]+r[M2])*sin(r[PHI1]-r[PHI2])*pow(r[DPHI1],2)-(4*r[M1]+3*r[M2]-r[M2]*cos(2*(r[PHI1]-r[PHI2])))*r[DR]*r[DPHI2]))/(2.*(r[M1]+r[M2]-r[M2]*pow(cos(r[PHI1]-r[PHI2]),2))*pow(r[R],2));
	drdt[R] = r[DR];
	drdt[PHI1] = r[DPHI1];
	drdt[PHI2] = r[DPHI2];
	drdt[R0] = 0;
	drdt[G] = 0;
	drdt[K] = 0;
	drdt[L] = 0;
	drdt[M1] = 0;
	drdt[M2] = 0;
}

void integrate_springy_pendulum(double *r, double dt) {
	runge_kutta_4(derivs_springy_pendulum, r, dt, 12);
}

double U_springy_pendulum(double *r) {
	return(r[G]*r[L]*(r[M1]+r[M2])*(1-cos(r[PHI1]))+r[G]*r[M2]*(1-cos(r[PHI2]))*r[R]+(r[K]*pow(-r[R0]+r[R],2))/2.);
}

double T_springy_pendulum(double *r) {
	return((pow(r[L],2)*(r[M1]+r[M2])*pow(r[DPHI1],2))/2.+(r[M2]*(pow(r[DR],2)+2*r[L]*cos(r[PHI1]-r[PHI2])*r[R]*r[DPHI1]*r[DPHI2]+pow(r[R],2)*pow(r[DPHI2],2)))/2.);
}

double lower_flip_springy_pendulum(double *r, double *r0, 
		double t, double *values, int done) {
	if ( ! done ) {
		return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
	} else {
		return(t);
	}	
}

double upper_flip_springy_pendulum(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI1] > 2*M_PI) || (r[PHI1] < -2*M_PI) );
    } else {
        return(t);
    }
}

