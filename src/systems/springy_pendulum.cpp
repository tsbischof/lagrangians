#include <math.h>

#include "springy_pendulum.hpp"
#include "../integrators.hpp"

namespace lagrangians 
{ 
	namespace springy_pendulum 
	{
	void derivatives
	(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c)
	{
		drdt[DR] = -c[G]+(c[K]*c[R_0])/c[M2]+c[G]*cos(r[PHI2])+c[L]*cos(r[PHI1]-r[PHI2])*r[DPHI1]*r[DPHI2]+r[R]*(-(c[K]/c[M2])+pow(r[DPHI2],2));
		drdt[DPHI1] = (c[L]*c[M2]*pow(cos(r[PHI1]-r[PHI2]),2)*r[DR]*r[DPHI1]-c[M2]*pow(r[R],2)*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2)-(r[R]*(2*c[G]*c[M1]*sin(r[PHI1])+c[G]*c[M2]*sin(r[PHI1])+c[G]*c[M2]*sin(r[PHI1]-2*r[PHI2])+c[L]*c[M2]*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI1],2)-2*c[M2]*cos(r[PHI1]-r[PHI2])*r[DR]*r[DPHI2]))/2.)/(c[L]*(c[M1]+c[M2]-c[M2]*pow(cos(r[PHI1]-r[PHI2]),2))*r[R]);
		drdt[DPHI2] = (-2*c[L]*(c[M1]+c[M2])*cos(r[PHI1]-r[PHI2])*r[DR]*r[DPHI1]+c[M2]*pow(r[R],2)*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI2],2)+r[R]*(2*c[G]*(c[M1]+c[M2])*cos(r[PHI1])*sin(r[PHI1]-r[PHI2])+2*c[L]*(c[M1]+c[M2])*sin(r[PHI1]-r[PHI2])*pow(r[DPHI1],2)-(4*c[M1]+3*c[M2]-c[M2]*cos(2*(r[PHI1]-r[PHI2])))*r[DR]*r[DPHI2]))/(2.*(c[M1]+c[M2]-c[M2]*pow(cos(r[PHI1]-r[PHI2]),2))*pow(r[R],2));
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
	} // springy_pendulum
} //lagrangians
