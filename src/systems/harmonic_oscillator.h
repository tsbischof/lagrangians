#ifndef HARMONIC_OSCILLATOR_H_
#define HARMONIC_OSCILLATOR_H_

double force_harmonic_oscillator(double *r);
void integrate_harmonic_oscillator(double *r, double dt);
int do_harmonic_oscillator(char *config_filename);

#endif 
