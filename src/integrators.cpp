#include "integrators.hpp"

namespace lagrangians 
{
	void runge_kutta_4
	(std::vector<double> &r, std::vector<double> const& c, void (*derivative)(std::vector<double> &, std::vector<double> const&, std::vector<double> const&), double const dt)
	{
		int i;
		std::vector<double> k1(r.size(), 0), k2(r.size(), 0), k3(r.size(), 0), k4(r.size(), 0), yt(r.size(), 0);
		std::vector<double> dydx(r.size()), dydxt(r.size());

		derivative(dydx, r, c);
		for ( i = 0; i < r.size(); i++ ) {
			k1[i] = dt*dydx[i];
	       		yt[i] = r[i] + 0.5*k1[i];
		}

		derivative(dydx, yt, c);
		for ( i = 0; i < r.size(); i++ ) {
			k2[i] = dt*dydx[i];
			yt[i] = r[i] + 0.5*k2[i];
		}

		derivative(dydx, yt, c);
		for ( i = 0; i < r.size(); i++ ) {
			k3[i] = dt*dydxt[i];
			yt[i] = r[i] + k3[i];
		}

		derivative(dydx, yt, c);
		for ( i = 0; i < r.size(); i++ ) {
			k4[i] = dt*dydxt[i];
		}
		
		for ( i = 0; i < r.size(); i++ ) {
			r[i] += 1.0/6.0*(k1[i] + 2*k2[i] + 2*k3[i] + k4[i]);
		}
	}

} // lagrangians
