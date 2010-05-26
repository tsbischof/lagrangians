#ifndef DOUBLE_PENDULUM_H_
#define DOUBLE_PENDULUM_H_

void derivs_double_pendulum(double *r, double *drdt);
void integrate_double_pendulum(double *r, double dt);
int do_double_pendulum(char *config_filename);

#endif
