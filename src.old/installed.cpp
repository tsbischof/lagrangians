#include <vector>
#include <map>
#include <iostream>
#include <boost/ref.hpp>

#include "harmonic_oscillator.hpp"
#include "double_well.hpp"
#include "double_pendulum.hpp"
#include "rules.hpp"

using namespace std;

void (*get_dispatcher(string integrator, bool *result))(string) {
	if ( integrator == "harmonic_oscillator" ) {
		*result = true;
		return(do_harmonic_oscillator);
	} else if ( integrator == "double_well" ) {
		*result = true;
		return(do_double_well);
	} else if ( integrator == "double_pendulum" ) {
		*result = true;
		return(do_double_pendulum);
	} else {
		*result = false;
		return(do_harmonic_oscillator);
	}
}

void (*get_integrator(string integrator, bool *result))(vector<double> *, double) {
	if ( integrator == "harmonic_oscillator" ) {
		return(integrate_harmonic_oscillator);
	} else if ( integrator == "double_well" ) {
		return(integrate_double_well);
	} else if ( integrator == "double_pendulum" ) {
		return(integrate_double_pendulum);
	} else {
		cout << "Fatal: Integrator " << integrator << " is not installed." << endl;
		return(integrate_harmonic_oscillator);
	}
}

bool (*get_rule(string rule, bool *result))(vector<double> *, vector<double> *) {
	if ( rule == "first_turnaround" ) {
		return(first_turnaround);
	} else if ( rule == "first_flip" ) {
		return(first_flip);
	} else {
		cout << "Fatal: Rule " << rule << " is not installed." << endl;
		return(first_flip);
	}
}
