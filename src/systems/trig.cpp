#include <math.h>

#include "trig.hpp"
#include "../integrators.hpp"
#include "../util.hpp"

namespace lagrangians
{
	namespace trig
	{
		void derivatives
		(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c)
		{
			drdt[V] = -c[K1]*r[X] -c[K2]*r[V] + c[K3]*cos(r[X]) + c[K4]*cos(r[V]) + c[K5]*sin(r[X]) + c[K6]*sin(r[V]);
			drdt[X] = r[V];
		}

		void integrate
		(std::vector<double>& r, std::vector<double> const& c, double const dt)
		{
			runge_kutta_4(r, c, derivatives, dt);
		}

		bool first_turnaround
		(std::vector<double> const& r, std::vector<double> const& r0)
		{
			return( (r[V] == 0) or (sign(r[V]) == -sign(r0[V])) );
		}
	} // trig
} // lagrangians
