#ifndef GRAPHER_HPP_
#define GRAPHER_HPP_

#include <vector>
#include <sstream>
#include <omp.h>

using namespace std;

class Grapher {
	void (*integrate)(vector<double> *, double); // void *integrate(double r[], double dt)
	bool (*rule)(vector<double> *, vector<double> *); // void *rule(double r[], double r0[]);
	string parm1;
	string parm2;
	multimap<string, vector<double> > parms; // array with entries {name, limits}
	vector<string> parm_order; // array determining the order of the parameters when passed to the integrator
	string name;
	string comment;
public:
	vector<vector<double> > image;
	void do_image(void);
	void set_integrator(void (*f)(vector<double> *, double)) {integrate = f;}
	void set_rule(bool (*r)(vector<double> *, vector<double> *)) {rule = r;}
	void set_x(string parm) {parm1 = parm;}
	void set_y(string parm) {parm2 = parm;}
	void set_parms(multimap<string, vector<double> > parameters) {parms = parameters;}
	void set_parm_order(vector<string> order) {parm_order = order;}
	void set_name(string myname) {name = myname;}
	void set_comment(string mycomment) {comment = mycomment;}
	void to_raw(void);
	void to_ppm(void);
private:
	int pixels(string);
};

#endif
