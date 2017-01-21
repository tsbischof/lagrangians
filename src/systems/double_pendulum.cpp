#include <math.h>

#include "double_pendulum.hpp"
#include "../integrators.hpp"
#include "../util.hpp"

namespace lagrangians
{

namespace double_pendulum {

void derivatives
(std::vector<double> &drdt, std::vector<double> const& r, std::vector<double> const& c)
{
	drdt[PHI1] = r[DPHI1];
	drdt[DPHI1] = ( - c[G]*((2*c[M1]+c[M2])*sin(r[PHI1])
		+ c[M2]*sin(r[PHI1]-2*r[PHI2]))
		- c[L1]*c[M2]*sin(2*(r[PHI1]-r[PHI2]))*pow(r[DPHI1],2)
		- 2*c[L2]*c[M2]*sin(r[PHI1]-r[PHI2])*pow(r[DPHI2],2))
		/ (2*c[L1]*(c[M1]+c[M2]*(1-pow(cos(r[PHI1]-r[PHI2]),2))));
	drdt[PHI2] = r[DPHI2];
	drdt[DPHI2] = (sin(r[PHI1]-r[PHI2]) 
		* (c[G]*(c[M1]+c[M2])*cos(r[PHI1])
		   +c[L1]*(c[M1]+c[M2])*pow(r[DPHI1],2)
		   +c[L2]*c[M2]*cos(r[PHI1]-r[PHI2])*pow(r[DPHI2],2)))
		/ (c[L2]*(c[M1]+c[M2]*(1-pow(cos(r[PHI1]-r[PHI2]),2))));
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

bool lower_turnaround_endpoint
(std::vector<double> const& r, std::vector<double> const& r0)
{
	return( (r[DPHI2] == 0) or (lagrangians::sign(r[DPHI2]) == -1*lagrangians::sign(r0[DPHI2])) );
}

bool upper_turnaround_endpoint
(std::vector<double> const& r, std::vector<double> const& r0)
{
	return( (r[DPHI1] == 0) or (lagrangians::sign(r[DPHI1]) == -1*lagrangians::sign(r0[DPHI1])) );
}

} // double_pendulum
} // lagrangians
