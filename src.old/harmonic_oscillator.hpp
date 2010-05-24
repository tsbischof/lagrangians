#ifndef HARMONIC_OSCILLATOR_HPP_
#define HARMONIC_OSCILLATOR_HPP_

#include <vector>
#include <string>
#include <boost/program_options.hpp>
namespace po = boost::program_options;

using namespace std;

double force_harmonic_oscillator(double *r);
void integrate_harmonic_oscillator(double *r, double dt);
void do_harmonic_oscillator(string config_filename,
		po::options_description *options);

#endif /* HARMONIC_OSCILLATOR_HPP_ */
