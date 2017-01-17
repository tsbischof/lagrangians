#ifndef INTEGRATORS_HPP_
#define INTEGRATORS_HPP_

#include <vector>

namespace lagrangians 
{
	void runge_kutta_4(std::vector<double>& r, std::vector<double> const& c, void (*derivative)(std::vector<double> &, std::vector<double> const&, std::vector<double> const&), double const t);
}

#endif
