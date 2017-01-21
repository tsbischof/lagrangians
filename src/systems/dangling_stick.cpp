#include <math.h>

#include "dangling_stick.hpp"
#include "../integrators.hpp"

/* Simulates the motion of a massless stick with a mass on either end, attached
 * to a spring. Descriptions of parameters:
 * r: length of the spring
 * phi1: angle of the spring, relative to vertical.
 * phi2: angle of the stick, relative to vertical.
 * m1: mass of the mass connected to the spring
 * m2: mass of the mass dangling from the stick
 * k: spring constant for the spring
 * l: length of the stick
 * A `d' in front of the variable indicates a time derivative.
 */

namespace lagrangians 
{ 
	namespace dangling_stick 
	{
	void derivatives
	(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c)
	{
		drdt[DR] = (-2*c[G]*pow(c[M1],2)-3*c[G]*c[M1]*c[M2]-c[G]*pow(c[M2],2)+2*c[K]*c[M1]*c[R_0]+c[K]*c[M2]*c[R_0]+2*c[G]*pow(c[M1],2)*cos(r[PHI1])+2*c[G]*c[M1]*c[M2]*cos(r[PHI1])+c[G]*c[M1]*c[M2]*cos(2*(r[PHI1]-r[PHI2]))+r[G]*pow(c[M2],2)*cos(2*(r[PHI1]-r[PHI2]))-c[K]*c[M2]*c[R_0]*cos(2*(r[PHI1]-r[PHI2]))+r[R]*(-(c[K]*(2*c[M1]+c[M2]-c[M2]*cos(2*(r[PHI1]-r[PHI2]))))+2*c[M1]*(c[M1]+c[M2])*pow(r[DPHI1],2))+2*c[L]*c[M1]*c[M2]*cos(r[PHI1]-r[PHI2])*pow(r[DPHI2],2))/(2.*c[M1]*(c[M1]+c[M2]));
		drdt[DPHI1] = -(2*c[G]*pow(c[M1],2)*sin(r[PHI1])+2*c[G]*c[M1]*c[M2]*sin(r[PHI1])+c[G]*c[M1]*c[M2]*sin(2*(r[PHI1]-r[PHI2]))+c[G]*pow(c[M2],2)*sin(2*(r[PHI1]-r[PHI2]))-c[K]*c[M2]*c[R_0]*sin(2*(r[PHI1]-r[PHI2]))+c[K]*c[M2]*r[R]*sin(2*(r[PHI1]-r[PHI2]))+4*c[M1]*(c[M1]+c[M2])*r[DR]*r[DPHI1]+2*c[L]*c[M1]*c[M2]*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2))/(2.*c[M1]*(c[M1]+c[M2])*r[R]);
		drdt[DPHI2] = ((c[G]*(c[M1]+c[M2])-c[K]*c[R_0]+c[K]*r[R])*sin(r[PHI1]-r[PHI2]))/(c[L]*c[M1]);
		drdt[R] = r[DR];
		drdt[PHI1] = r[DPHI1];
		drdt[PHI2] = r[DPHI2];
	}

	void integrate
	(std::vector<double>& r, std::vector<double> const& c, double const dt)
	{
		runge_kutta_4(r, c, derivatives, dt);
	}

	bool lower_flip_endpoint
	(std::vector<double> const& r, std::vector<double> const& r0)
	{
		return( (r[PHI2] > 2*M_PI) or (r[PHI2] < -2*M_PI) );
	}

	bool upper_flip_endpoint
	(std::vector<double> const& r, std::vector<double> const& r0)
	{
		return( (r[PHI1] > 2*M_PI) or (r[PHI1] < -2*M_PI) );
	}
	} // dangling_stick
} //lagrangians
