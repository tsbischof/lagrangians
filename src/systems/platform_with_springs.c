#include "platform_with_springs.h"

/* Simulates the motion of a (2D) block with length and height, rotating about
 * a fixed axis perpendicular to the plane of the block. At either end is a 
 * spring with a mass attached.
 * r1: length of the left spring
 * r10: natural length of the left spring
 * k1: spring constant of the left spring
 * m1: mass dangling from the left spring
 * phi1: angle of the left spring, relative to vertical
 * r2, r20, k2, m2, phi2: ditto
 * b, h: base and height of the block
 * x0, y0: the origin about which the system rotates
 * m3: mass of the block
 * phi3: angle of the block, relative to vertical
 */

enum { R1, DR1, R10, K1, M1, PHI1, DPHI1, 
		R2, DR2, R20, K2, M2, PHI2, DPHI2,
		B, H, X0, Y0, M3, PHI3, DPHI3, G};

void do_platform_with_springs(dictionary *options, Grapher *grapher) {
	Functions functions;
	functions.integrate_names[0] = "platform_with_springs";
	functions.integrate_funcs[0] = integrate_platform_with_springs;

	functions.rule_names[0] = "left_flip";
	functions.rule_funcs[0] = left_flip_platform_with_springs;
    functions.rule_names[1] = "right_flip";
    functions.rule_funcs[1] = right_flip_platform_with_springs;
    functions.rule_names[2] = "platform_flip";
    functions.rule_funcs[2] = platform_flip_platform_with_springs;

    char *variable_order[22] = {"r1", "dr1", "r10", "k1", "m1", "phi1", "dphi1",
        "r2", "dr2", "r20", "k2", "m2", "phi2", "dphi2",
        "b", "h", "x0", "y0", "m3", "phi3", "dphi3", "g"};

	double variable_defaults[22] = {1, 0, 1, 1, 1, 0, 0,
		1, 0, 1, 1, 1, 0, 0, 
		4, 1, 0, 0, 1, 0, 0, 9.8};
    setup_config(grapher, options, &variable_order[0], &variable_defaults[0],
				22, &functions);
}

void derivs_platform_with_springs(double *r, double *drdt) {
}

void integrate_platform_with_springs(double *r, double dt) {
	runge_kutta_4(derivs_platform_with_springs, r, dt, 22);
}

double U_platform_with_springs(double *r) {
	return(r[G]*sin(r[PHI3])*r[M3]*(pow(r[X0],2)+pow(r[Y0],2))+r[G]*(1-cos(r[PHI1]))*r[M1]*r[R1]+(r[K1]*pow(-r[R10]+r[R1],2))/2.+r[G]*(1-cos(r[PHI2]))*r[M2]*r[R2]+(r[K2]*pow(-r[R20]+r[R2],2))/2.);
}

double T_platform_with_springs(double *r) {
	return(((((pow(r[B],2)+pow(r[H],2))*r[M3])/12.+r[M3]*(pow(r[X0],2)+pow(r[Y0],2)))*pow(r[PHI3],2))/2.+(r[M2]*(pow(-(cos(r[PHI2])*r[DR2])+sin(r[PHI2])*r[R2]*r[DPHI2]+(r[B]*cos(r[PHI3])*r[DPHI3])/2.,2)+pow(sin(r[PHI2])*r[DR2]+cos(r[PHI2])*r[R2]*r[DPHI2]-(r[B]*sin(r[PHI3])*r[DPHI3])/2.,2)))/2.+(r[M1]*(pow(-(cos(r[PHI1])*r[DR1])+sin(r[PHI1])*r[R1]*r[DPHI1]-(r[B]*cos(r[PHI3])*r[DPHI3])/2.,2)+pow(sin(r[PHI1])*r[DR1]+cos(r[PHI1])*r[R1]*r[DPHI1]+(r[B]*sin(r[PHI3])*r[DPHI3])/2.,2)))/2.);
}

double left_flip_platform_with_springs(double *r, double *r0, 
		double t, double *values, int done) {
	if ( ! done ) {
		return( (r[PHI1] > 2*M_PI) || (r[PHI1] < -2*M_PI) );
	} else {
		return(t);
	}	
}

double right_flip_platform_with_springs(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
    } else {
        return(t);
    }
}

double platform_flip_platform_with_springs(double *r, double *r0,
        double t, double *values, int done) {
    if ( ! done ) {
        return( (r[PHI3] > 2*M_PI) || (r[PHI3] < -2*M_PI) );
    } else {
        return(t);
    }
}
