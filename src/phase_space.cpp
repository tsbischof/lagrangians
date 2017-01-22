#include <iostream>
#include <stdexcept>

#include "phase_space.hpp"

using namespace lagrangians;

PhaseSpace::PhaseSpace
(std::vector<double> const origin,
 std::vector<double> const x, 
 std::vector<double> const y)
{
	if ( (origin.size() != x.size()) && (x.size() != y.size()) ) {
		throw(std::runtime_error("Vectors in the phase space must have the same dimensionality."));
	}

	// todo: make these calls into copies, to ensure that a local
	// version is kept as the master
	this->origin = origin;
	this->basis_x = x;
	this->basis_y = y;
}

std::vector<double> PhaseSpace::point
(double const a, double const b)
{
	std::vector<double> out;
	this->point(out, a, b);
	return(out);
}

void PhaseSpace::point
(std::vector<double>& r, double const a, double const b)
{
	for ( auto i = 0; i < this->origin.size(); i++ ) {
		r[i] = this->origin[i] + a*this->basis_x[i] + b*this->basis_y[i];
	}
}

/* void PhaseSpace::point
(boost::multi_array<double, 1>& r, double const a, double const b)
{
	std::vector<double> rv(r.size());
	this->point(rv, a, b);

	for ( auto i = 0; i < rv.size(); i++ ) {
		r[i] = rv[i];
	}
} */
