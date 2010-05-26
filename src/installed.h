#ifndef INSTALLED_HPP_
#define INSTALLED_HPP_

int get_dispatcher(char *integrator, 
                   int (*dispatcher)(char *));
int get_integrator(char *integrator, 
                   void (*integrator_func)(double *, double));
int get_rule(char *rule, int (*rule_func)(double *, double *));

#endif /* INSTALLED_HPP_ */
