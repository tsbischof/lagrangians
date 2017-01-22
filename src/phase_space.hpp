#ifndef PHASE_SPACE_HPP_
#define PHASE_SPACE_HPP_

#include <vector>

#include <boost/multi_array.hpp>

namespace lagrangians
{
	class PhaseSpace
	{
		std::vector<double> origin;
		std::vector<double> basis_x;
		std::vector<double> basis_y;

		public:
			PhaseSpace(std::vector<double> const origin, std::vector<double> const basis_x, std::vector<double> const basis_y);

			std::vector<double> point(double const a, double const b);
			void point(std::vector<double> &r, double const a, double const b);
			void point(boost::multi_array<double, 1> &r, double const a, double const b);
	};
} // lagrangians
#endif
