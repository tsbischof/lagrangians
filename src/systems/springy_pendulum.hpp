#ifndef SPRINGY_PENDULUM_HPP_
#define SPRINGY_PENDULUM_HPP_

#include <string>
#include <vector>

namespace lagrangians
{
	namespace springy_pendulum
	{
		enum { R, DR, PHI1, DPHI1, PHI2, DPHI2 };
		enum { M1, M2, R_0, K, L, G };
		static std::vector<std::string> const variables({"r", "dr", "phi1", "dphi1", "phi2", "dphi2"});
		static std::vector<std::string> const constants({"m1", "m2", "r_0", "k", "l", "g"});

		void derivatives(std::vector<double> &drdt, std::vector<double> const& r, std::vector<double> const& c);
		void integrate(std::vector<double>& r, std::vector<double> const& c, double const dt);
		bool lower_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
		bool upper_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
	} // springy_pendulum
} // lagrangians

#endif
