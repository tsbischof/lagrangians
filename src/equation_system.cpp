#include <iostream>

#include <boost/algorithm/string.hpp>

#include "equation_system.hpp"
#include "systems/dangling_stick.hpp"
#include "systems/double_pendulum.hpp"
#include "systems/double_spring.hpp"
#include "systems/double_well.hpp"
#include "systems/trig.hpp"

using namespace lagrangians;

EquationSystem::EquationSystem
(std::string const system, std::string const endpoint, std::string const validator)
{
	// all validations fail by default
	this->validate = [](std::vector<double> const &) { return(false); };
	this->endpoint = [](std::vector<double> const &, std::vector<double> const &) { return(false); };
	auto not_found = [](std::string system, std::string endpoint){ throw(std::runtime_error("Endpoint \"" + endpoint + "\" not found for " + system)); };

	if ( system == "dangling_stick" ) {
		this->constants = dangling_stick::constants;
		this->variables = dangling_stick::variables;
		this->integrate = dangling_stick::integrate;

		if ( endpoint == "" ) {
			// nothing, default is in place
		} else if ( endpoint == "lower_flip" ) {
			this->endpoint = dangling_stick::lower_flip_endpoint;
		} else if ( endpoint == "upper_flip" ) {
			this->endpoint = dangling_stick::upper_flip_endpoint;
		} else {
			not_found(system, endpoint);
		}
	} else if ( system == "double_pendulum" ) {
		this->constants = double_pendulum::constants;
		this->variables = double_pendulum::variables;
		this->integrate = double_pendulum::integrate;

		if ( endpoint == "" ) {
			// nothing, default is in place
		} else if ( endpoint == "lower_flip" ) {
			this->endpoint = double_pendulum::lower_flip_endpoint;
		} else if ( endpoint == "upper_flip" ) {
			this->endpoint = double_pendulum::upper_flip_endpoint;
		} else if ( endpoint == "lower_turnaround" ) {
			this->endpoint = double_pendulum::lower_turnaround_endpoint;
		} else if ( endpoint == "upper_turnaround" ) {
			this->endpoint = double_pendulum::upper_turnaround_endpoint;
		} else {
			not_found(system, endpoint);
		}
	} else if ( system == "double_spring" ) {
		this->constants = double_spring::constants;
		this->variables = double_spring::variables;
		this->integrate = double_spring::integrate;

		if ( endpoint == "" ) {
			// nothing, default is in place
		} else if ( endpoint == "lower_flip" ) {
			this->endpoint = double_spring::lower_flip_endpoint;
		} else if ( endpoint == "upper_flip" ) {
			this->endpoint = double_spring::upper_flip_endpoint;
		} else {
			not_found(system, endpoint);
		}
	} else if ( system == "double_well" ) {
		this->constants = double_well::constants;
		this->variables = double_well::variables;
		this->integrate = double_well::integrate;

		if ( endpoint == "" ) {
			// nothing, default is in place
		} else if ( endpoint == "first_turnaround" ) {
			this->endpoint = double_well::first_turnaround;
		} else {
			not_found(system, endpoint);
		}
	} else if ( system == "trig" ) {
		this->constants = trig::constants;
		this->variables = trig::variables;
		this->integrate = trig::integrate;

		if ( endpoint == "" ) {
			// nothing, default is in place
		} else if ( endpoint == "first_turnaround" ) {
			this->endpoint = trig::first_turnaround;
		} else {
			not_found(system, endpoint);
		}
	} else {
		throw(std::runtime_error("System not found: " + system));
	}
}

void EquationSystem::get_origin_and_basis
(std::vector<double>& origin, std::vector<double>& basis_x, std::vector<double>& basis_y, std::map<std::string, std::string> const default_parameters, std::map<std::string, std::string> const horizontal_parameters, std::map<std::string, std::string> const vertical_parameters)
{
	int i;
	bool in_default, in_x, in_y;
	std::string name;
	std::vector<std::string> values;

	// check that parameters are not found in the horizontal and 
	// vertical definitions
	
	// generate the origin by taking the default, horizontal, 
	// or vertical
	
	// if any values are not specified, complain
	for ( i = 0; i < (this->variables.size() + this->constants.size()); i++ ) {
		in_default = false;
		in_x = false;
		in_y = false;

		origin[i] = 0;
		basis_x[i] = 0;
		basis_y[i] = 0;

		if ( i < this->variables.size() ) {
			name = this->variables[i];
		} else {
			name = this->constants[i - this->variables.size() ];
		}

		if ( default_parameters.count(name) ) {
			in_default = true;
			origin[i] = std::stof(default_parameters.at(name));
			basis_x[i] = 0;
			basis_y[i] = 0;
		} 
		
		if ( horizontal_parameters.count(name) ) {
			in_x = true;
			boost:split(values, horizontal_parameters.at(name), boost::is_any_of(","));
			origin[i] = std::stof(values[0]);
			basis_x[i] = std::stof(values[1]) - origin[i];
		} 
		
		if ( vertical_parameters.count(name) ) {
			in_y = true;
			boost::split(values, vertical_parameters.at(name), boost::is_any_of(","));

			if ( in_x ) { 
				// we have this value on both axes, so we need
				// to make sure the origin is the same
				if ( origin[i] != std::stof(values[0]) ) {
					throw(std::runtime_error("Parameter " + name + " given in both the horizontal and vertical axes, but their origins are not the same. Ensure the first value for each axis is the same."));
				}
			}

			origin[i] = std::stof(values[0]);
			basis_y[i] = std::stof(values[1]) - origin[i];
		} 

		if ( (in_default and in_x) or (in_default and in_y) ) {
			throw(std::runtime_error("Parameters cannot be specified in both defaults and an axis: " + name));
		} 

		if ( not (in_default or in_x or in_y) ) {
			throw(std::runtime_error("Parameter not specified in input file: " + name + ". All parameters must be given a value."));
		}
	}
}

size_t EquationSystem::n_parameters
(void)
{
	return(this->variables.size() + this->constants.size());
}
