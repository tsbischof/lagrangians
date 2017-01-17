#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <map>
#include <vector>

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace lagrangians {
	void get_section(std::map<std::string, std::string>& parameters, pt::ptree const& tree);
	double sign(double const x);
	void split(std::vector<std::string>& elems, std::string const& s, char delim);
	std::vector<std::string> split(std::string const& s, char delim);
}
#endif
