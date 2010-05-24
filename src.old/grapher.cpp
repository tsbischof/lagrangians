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

	vector<double> t_limits = Grapher::parms.find("t")->second;
	double t_limit = t_limits[2];
	double dt = t_limits[1];

	int vec_length = Grapher::parm_order.size();

	double t;
	double r[vec_length], r0[vec_length];
	double *rptr, *r0ptr;

#pragma omp parallel for private(r, r0, rptr, r0ptr, t)
	for ( i = 0; i < height; i++) {
		for ( j = 0; j < width; j++) {
			if ( ++k % 1000 == 0 ) {
				cout << "On pixel " << k << " of " 
					 << height*width << "." << endl;
			}

			t = t_limits[0];
			vector<double> limits;
			double val;

			for (int l = 0; l < vec_length; l++) {
				if ( parm_order[l] == Grapher::parm1 ) {
					limits = Grapher::parms.find(parm_order[l])->second;
					val = limits[0] + limits[1] * i;
				} else if ( parm_order[l] == Grapher::parm2 ) {
					limits = Grapher::parms.find(parm_order[l])->second;
					val = limits[0] + limits[1] * j;
				} else {
					val = 0;
				}
				r[l] = val;
				r0[l] = val;
			}

			rptr = r;
			r0ptr = r0;

			while (t <= t_limit) {
				Grapher::integrate(rptr, dt);
				if ( Grapher::rule(rptr, r0ptr) ) {
					break;
				} else {
					t += dt;
				}
			}

			Grapher::image[i][j] = t;
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
