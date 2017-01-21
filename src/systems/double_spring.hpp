#ifndef DOUBLE_SPRING_HPP_
#define DOUBLE_SPRING_HPP_

#include <string>
#include <vector>

namespace lagrangians 
{
	namespace double_spring 
	{
		enum { PHI1, DPHI1, PHI2, DPHI2, R1, DR1, R2, DR2 };
		enum { M1, M2, K1, K2, R1_0, R2_0, G };
		static std::vector<std::string> const variables({"phi1", "dphi1", "phi2", "dphi2", "r1", "dr1", "r2", "dr2"});
		static std::vector<std::string> const constants({"m1", "m2", "k1", "k2", "r1_0", "r2_0", "g"});

		void derivatives(std::vector<double> &drdt, std::vector<double> const& r, std::vector<double> const& c);
		void integrate(std::vector<double>& r, std::vector<double> const& c, double const dt);

		bool lower_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
		bool upper_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);

	} // double_spring
} // lagrangians
#endif
