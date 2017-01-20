#include <math.h>
#include <iostream>

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

double kinetic_energy
(std::vector<double> const& r) 
{
	return(1.0/2*r[M1]*pow(r[DPHI1]*r[L1], 2) + 
		1.0/2*r[M2]*(pow(r[DPHI1]*r[L1], 2)+pow(r[DPHI2]*r[L2], 2)+
		 2*r[L1]*r[L2]*r[DPHI1]*r[DPHI2]*cos(r[PHI1]-r[PHI2])));
}

double potential_energy
(std::vector<double> const& r)
{
	return(r[M1]*r[G]*r[L1]*(1-cos(r[PHI1]))+r[M2]*r[G]*(r[L1]*(1-cos(r[PHI1]))+r[L2]*(1-cos(r[PHI2]))));
}

bool lower_flip_energy_validator
(std::vector<double> const& r)
{
	std::vector<double> r_min({0, M_PI, 0, 0, r[M1], r[M2], r[L1], r[L2], r[G]});

	return( potential_energy(r) + kinetic_energy(r) > potential_energy(r_min) );
}

bool lower_flip_endpoint
(std::vector<double> const& r, std::vector<double> const& r0)
{
	return( (r[PHI2] > 2*M_PI) || (r[PHI2] < -2*M_PI) );
}

bool lower_turnaround_endpoint
(std::vector<double> const& r, std::vector<double> const& r0)
{
	return( (r[DPHI2] == 0) || (lagrangians::sign(r[DPHI2]) == -1*lagrangians::sign(r0[DPHI2])) );
}

bool upper_turnaround_endpoint
(std::vector<double> const& r, std::vector<double> const& r0)
{
	return( (r[DPHI1] == 0) || (lagrangians::sign(r[DPHI1]) == -1*lagrangians::sign(r0[DPHI1])) );
}

} // double_pendulum
} // lagrangians
