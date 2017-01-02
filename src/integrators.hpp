#ifndef INTEGRATORS_H_
#define INTEGRATORS_H_

#include <vector>

namespace lagrangians 
{
	void runge_kutta_4(std::vector &, void (&)(std::vector<double> &, std::vector<double> &), double const &);
}

#endif
