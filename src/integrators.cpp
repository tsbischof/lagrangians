#include "integrators.hpp"

namespace lagrangians 
{
	void runge_kutta_4
	(std::vector<double> &r, void (*derivs)(std::vector<double> &, std::vector<double> const&), double const dt)
	{
		std::vector<double> k1, k2, k3, k4, yt;
		std::vector<double> dydx, dydxt;

/*		derivs(r, dydx);
		k1 = dt*dydx;
		yt = r + 0.5*k1;

		derivs(yt, dydxt);
		k2 = dt*dydxt;
		yt = r + 0.5*k2;

		derivs(yt, dydxt);
		k3 = dt*dydxt;
		yt = r + k3;

		derivs(yt, dydxt);
		k4 = dt*dydxt;
		
		r += 1.0/6.0*(k1 + 2*k2 + 2*k3 + k4); */
	}

} // lagrangians
