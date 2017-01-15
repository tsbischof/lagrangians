#ifndef DOUBLE_PENDULUM_HPP_
#define DOUBLE_PENDULUM_HPP_

#include <string>
#include <vector>

namespace lagrangians::double_pendulum
{
	enum { PHI1, DPHI1, PHI2, DPHI2, M1, M2, L1, L2, G };
	static std::vector<std::string> const parameters({"phi1", "dphi1", "phi2", "dphi2", "m1", "m2", "l1", "l2", "g"});

	void derivatives(std::vector<double> &drdt, std::vector<double> const& r);
	void integrate(std::vector<double>& r, double const dt);
	double kinetic_energy(std::vector<double> const& r);
	double potential_energy(std::vector<double> const& r);
	bool lower_flip_energy_validator(std::vector<double> const& r);
	bool lower_flip_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
	bool lower_turnaround_endpoint(std::vector<double> const& r, std::vector<double> const& r0);
	bool upper_turnaround_endpoint(std::vector<double> const& r, std::vector<double> const& r0);

} // lagrangians::double_pendulum
#endif
