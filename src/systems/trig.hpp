#ifndef TRIG_HPP_
#define TRIG_HPP_

#include <string>
#include <vector>

namespace lagrangians
{
	namespace trig
	{
		enum { X, V };
		enum { K1, K2, K3, K4, K5, K6 };
		static std::vector<std::string> const variables({"x", "v"});
		static std::vector<std::string> const constants({"k1", "k2", "k3", "k4", "k5", "k6"});

		void derivatives(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c);
		void integrate(std::vector<double>& r, std::vector<double> const& c, double const dt);
		
		bool first_turnaround(std::vector<double> const& r, std::vector<double> const& r0);
	} // trig

} // lagrangians

#endif
