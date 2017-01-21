#ifndef DOUBLE_WELL_HPP_
#define DOUBLE_WELL_HPP_

#include <string>
#include <vector>

namespace lagrangians
{
	namespace double_well
	{
		enum { X, V };
		enum { K1, K2 };
		static std::vector<std::string> const variables({"x", "v"});
		static std::vector<std::string> const constants({"k1", "k2"});

		void derivatives(std::vector<double>& drdt, std::vector<double> const& r, std::vector<double> const& c);
		void integrate(std::vector<double>& r, std::vector<double> const& c, double const dt);
		
		bool first_turnaround(std::vector<double> const& r, std::vector<double> const& r0);
	} // double_well

} // lagrangians

#endif
