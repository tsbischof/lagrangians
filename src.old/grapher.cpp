#include <iostream>
#include <math.h>
#include <map>
#include <omp.h>
#include <fstream>

#include "image_funcs.hpp"
#include "grapher.hpp"

using namespace std;

void Grapher::do_image(void) {
	/* Create the image. Allocate the pixel array and populate it using the
	 * integrator and rule.
	 */
	int width = Grapher::pixels(parm1);
	int height = Grapher::pixels(parm2);;

	cout << "Building image." << endl;
	cout << "Working with: " << parm1 << " and " << parm2 << endl;
	cout << "Dimensions: " << width << "x" << height << endl;

	Grapher::image.resize(height);

	int i,j;
	for ( i = 0; i < height; ++i) {
		Grapher::image[i].resize(width);
	}

	long int k = 0;
#pragma omp parallel for
	for ( i = 0; i < height; i++) {
		for ( j = 0; j < width; j++) {
			k++;
			if ( k % 1000 == 0 ) {
				cout << "On pixel " << k << " of " 
					 << height*width << "." << endl;
			}
			Grapher::image[i][j] = Grapher::do_pixel(i,j);
		}
	}

	cout << "Building image complete." << endl;
}

int Grapher::pixels(string name) {
	/* Tells how many pixels are required to span the limits with appropriate
	 * increments. 
	 */
	vector<double> lims;
	lims = parms.find(name)->second;
	return((int)ceil((lims[2]-lims[0])/lims[1]) + 1);
}

double Grapher::do_pixel(int parm1_index, int parm2_index) {
	/* Get the appropriate starting vector based on the pixel index, run
	 * the integrator until the rule is satisfied, and report the time.
	 */
	vector<double> t_limits = Grapher::parms.find("t")->second;
	double t = t_limits[0];
	double t_limit = t_limits[2];
	double dt = t_limits[1];

	// Allocate vectors for the computation, in the order prescribed.
	int vec_length = Grapher::parm_order.size();
	vector<double> r(vec_length,0), r0(vec_length,0);

	int i;
	vector<double> limits;
	double val;

	for (i = 0; i < vec_length; i++) {
		if ( parm_order[i] == Grapher::parm1 ) {
			limits = parms.find(parm_order[i])->second;
			val = limits[0] + limits[1] * parm1_index;
		} else if ( parm_order[i] == Grapher::parm2 ) {
			limits = parms.find(parm_order[i])->second;
			val = limits[0] + limits[1] * parm2_index;
		} else {
			val = 0;
		}
		r[i] = val;
		r0[i] = val;
	}

	while (t <= t_limit) {
		Grapher::integrate(&r, dt);
		if ( Grapher::rule(&r, &r0) ) {
			return(t);
		} else {
			t += dt;
		}
	}

	return(t);
}

void Grapher::to_raw(void) {
	string my_name = Grapher::name;
	my_name.append(".raw");
	image_to_raw(Grapher::image, my_name);
}


void Grapher::to_ppm(void) {
	string my_name = Grapher::name;
	my_name.append(".ppm");
	image_to_ppm(Grapher::image, my_name, Grapher::comment);
}
