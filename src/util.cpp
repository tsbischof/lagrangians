#include <iostream>

#include "util.hpp"

namespace lagrangians 
{
	void get_section
	(std::map<std::string, std::string>& parameters, pt::ptree const& tree)
	{
		for ( auto const& node: tree ) {
			parameters[node.first] = tree.get<std::string>(node.first);
		}
	} 

	double sign
	(double const x)
	{
		return( x < 0 ? -1 : 1 );
	}


	void split
	(std::vector<std::string>& elems, std::string const& s, char delim)
	{
		std::stringstream ss;
		ss.str(s);
		std::string item;
		
		while ( std::getline(ss, item, delim) ) {
			elems.push_back(item);
		}
	}

	std::vector<std::string> split
	(std::string const& s, char delim)
	{
		std::vector<std::string> elems;
		split(elems, s, delim);
		return(elems);
	} 
}
