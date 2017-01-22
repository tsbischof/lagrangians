#ifndef EQUATION_SYSTEM_HPP_
#define EQUATION_SYSTEM_HPP_

#include <map>
#include <string>
#include <vector>

#include <boost/multi_array.hpp>

namespace lagrangians
{
	class EquationSystem
	{
		public:
			EquationSystem(std::string const system, std::string const rule, std::string const validator);

			void get_origin_and_basis(std::vector<double>& origin, std::vector<double>& basis_x, std::vector<double>& basis_y, std::map<std::string, std::string> const default_parameters, std::map<std::string, std::string> const horizontal_parameters, std::map<std::string, std::string> const vertical_parameters);

			void (*integrate)(std::vector<double>& r, std::vector<double>const& c, double const dt);
			bool (*validate)(std::vector<double> const& r);
			bool (*endpoint)(std::vector<double> const& r, std::vector<double> const& r0);
			std::vector<std::string> constants;
			std::vector<std::string> variables;

			size_t n_parameters(void);
	};
} // lagrangians

#endif
