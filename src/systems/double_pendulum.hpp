#ifndef DOUBLE_PENDULUM_HPP_
#define DOUBLE_PENDULUM_HPP_

#include <string>
#include <vector>

namespace lagrangians 
{
	namespace double_pendulum 
	{
		enum { PHI1, DPHI1, PHI2, DPHI2 };
		enum { M1, M2, L1, L2, G };
		static std::vector<std::string> const variables({"phi1", "dphi1", "phi2", "dphi2"});
		static std::vector<std::string> const constants({"m1", "m2", "l1", "l2", "g"});

		void derivatives(std::vector<double> &drdt, std::vector<double> const& r, std::vector<double> const& c);
		void integrate(std::vector<double>& r, std::vector<double> const& c, double const dt);

		bool lower_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
		bool upper_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
		bool lower_turnaround_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
		bool upper_turnaround_endpoint(std::vector<double> const& r, std::vector<double> const& r0);

	} // double_pendulum
} // lagrangians
#endif
