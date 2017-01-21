#include <math.h>

#include "double_well.hpp"
#include "../integrators.hpp"
#include "../util.hpp"

namespace lagrangians
{
	namespace double_well
	{
		void derivatives
		(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c)
		{
			drdt[V] = -c[K1]*pow(r[X], 3) + c[K2]*r[X];
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
	} // double_well
} // lagrangians
