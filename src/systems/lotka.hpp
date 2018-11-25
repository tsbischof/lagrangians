#ifndef LOTKA_HPP_
#define LOTKA_HPP_

#include <string>
#include <vector>

namespace lagrangians
{
	namespace lotka
	{
		enum { X, Y };
		enum { ALPHA, BETA, DELTA, GAMMA };
		static std::vector<std::string> const variables({"x", "y"});
		static std::vector<std::string> const constants({"alpha", "beta", "delta", "gamma"});

		void derivatives(std::vector<double> &drdt, std::vector<double> const& r, std::vector<double> const& c);
		void integrate(std::vector<double>& r, std::vector<double> const& c, double const dt);
	} // lotka
} // lagrangians

#endif
