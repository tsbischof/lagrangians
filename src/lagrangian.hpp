#ifndef LAGRANGIAN_H_
#define LAGRANGIAN_H_

#include <string>
#include <boost/program_options.hpp>

namespace po = boost::program_options;

class Lagrangian
{
	std::string system;
	double time_limit;
	std::string halting_rule;
	
	public:
		Lagrangian();
		Lagrangian(std::string);
		Lagrangian(po::variables_map);
}

#endif
