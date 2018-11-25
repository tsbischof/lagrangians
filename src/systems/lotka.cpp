#include <math.h>

#include "lotka.hpp"
#include "../integrators.hpp"

/* Simulates the predatory-prey dynamics of the Lotka-Volterra equations
 * https://en.wikipedia.org/wiki/Lotka–Volterra_equations
 */

namespace lagrangians 
{ 
	namespace lotka
	{
		void derivatives
		(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c)
		{
			drdt[X] = c[ALPHA]*r[X] + c[BETA]*r[X]*r[Y];
			drdt[Y] = c[DELTA]*r[X]*r[Y] + c[GAMMA]*r[Y];
		}
	
		void integrate
		(std::vector<double>& r, std::vector<double> const& c, double const dt)
		{
			runge_kutta_4(r, c, derivatives, dt);
		}
	} // lotka
} //lagrangians
