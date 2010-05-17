#ifndef HARMONIC_OSCILLATOR_HPP_
#define HARMONIC_OSCILLATOR_HPP_

#include <vector>
#include <string>

using namespace std;

double force_harmonic_oscillator(vector<double> *r);
void integrate_harmonic_oscillator(vector<double> *r, double dt);
void do_harmonic_oscillator(string config_filename);

#endif /* HARMONIC_OSCILLATOR_HPP_ */
