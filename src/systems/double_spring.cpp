#include <math.h>

#include "double_spring.hpp"
#include "../integrators.hpp"
#include "../util.hpp"

namespace lagrangians
{

namespace double_spring {

void derivatives
(std::vector<double> &drdt, std::vector<double> const& r, std::vector<double> const& c)
{
	drdt[DR1] = (-(c[G]*c[M1])
			-c[G]*c[M2]
			+c[G]*c[M1]*cos(r[PHI1])
			+c[G]*c[M2]*cos(r[PHI1]-r[PHI2])
			+c[K1]*(c[R1_0]-r[R1])
			-cos(r[PHI1]-r[PHI2])*c[K2]*(c[R2_0]-r[R2])
			+c[M1]*r[R1]*pow(r[DPHI1],2))/c[M1];
	drdt[DR2] = (
			-(c[M2]*cos(r[PHI1]-r[PHI2])*c[K1]*(c[R1_0]-r[R1]))
			+(c[M1]+c[M2])*c[K2]*(c[R2_0]-r[R2])
			+c[M2]*(-2*c[G]*(c[M1]+c[M2])*pow(sin((r[PHI1]-r[PHI2])/2.),2)
			+c[M1]*r[R2]*pow(r[DPHI2],2)))
		/(c[M1]*c[M2]);
	drdt[DPHI1] = (-(c[G]*(c[M1]*sin(r[PHI1])
			+c[M2]*sin(r[PHI1]-r[PHI2])))
			+sin(r[PHI1]-r[PHI2])*c[K2]*(c[R2_0]-r[R2])
			-2*c[M1]*r[DR1]*r[DPHI1])
		/(c[M1]*r[R1]);
	drdt[DPHI2] = (c[G]*(c[M1]+c[M2])*sin(r[PHI1]-r[PHI2])
			-sin(r[PHI1]-r[PHI2])*c[K1]*(c[R1_0]-r[R1])
			-2*c[M1]*r[DR2]*r[DPHI2])
		/(c[M1]*r[R2]);
	drdt[R1] = r[DR1];
	drdt[R2] = r[DR2];
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

} // double_spring
} // lagrangians
