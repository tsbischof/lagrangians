#include <iostream>

#include "equation_system.hpp"

using namespace lagrangians;

EquationSystem::EquationSystem
(std::string const system, std::string const endpoint, std::string const validator)
{
	// all validations fail by default
	this->validate = [](std::vector<double> const &) { return(false); };

	if ( system == "dangling_stick" ) {
	} else if ( system == "double_pendulum" ) {
		this->parameters = double_pendulum::parameters;
		this->integrate = double_pendulum::integrate;

		if ( endpoint == "lower_flip" ) {
			this->endpoint = double_pendulum::lower_flip_endpoint;

			if ( validator == "energy" ) {
				this->validate = double_pendulum::lower_flip_energy_validator;
			}
		} else if ( endpoint == "lower_turnaround" ) {
			this->endpoint = double_pendulum::lower_turnaround_endpoint;
		} else if ( endpoint == "upper_turnaround" ) {
			this->endpoint = double_pendulum::upper_turnaround_endpoint;
		} else {
			throw(std::runtime_error("Endpoint \"" + endpoint + "\" not found for " + system));
		}
	} else {
		throw(std::runtime_error("System not found: " + system));
	}
}

void EquationSystem::get_origin_and_basis
(std::vector<double>& origin, std::vector<double>& basis_x, std::vector<double>& basis_y, std::map<std::string, std::string> const default_parameters, std::map<std::string, std::string> const horizontal_parameters, std::map<std::string, std::string> const vertical_parameters)
{
	// check that parameters are not found in the horizontal and 
	// vertical definitions
	
	// generate the origin by taking the default, horizontal, 
	// or vertical
	
	// if any values are not specified, complain
}
