#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <math.h>
#include <sstream>

#include "image_funcs.hpp"
#define RGB_SCALE 255 // maximum channel value

using namespace std;

string int_to_string(int n) {
	stringstream ss;
	ss << n;
	return(ss.str());
}

void image_to_ppm(vector<vector<double> > my_image,
		string filename, string desc) {
	int height = my_image.size();
	int width = my_image[0].size();

	cout << "Writing image to ppm file: " << filename << "." << endl;

	ofstream output;
	output.open(filename.c_str());
	output << "P3" << endl;
	output << "# " << desc << endl;
	output << width << " " << height << endl;
	output << RGB_SCALE << endl;

	cout << "Getting maximum pixel value." << endl;
	int i, j;
	double max_pixel = 0;
	for (i = 0; i < height; i++) {
//		cout << "Checking row " << i << " of " << height-1 << "." << endl;
		for (j = 0; j < width; j++) {
			if (max_pixel < my_image[i][j]) {
				max_pixel = my_image[i][j];
			}
		}
	}
	cout << "Found maximum pixel value of " << max_pixel << endl;

	for (i = 0; i < height; i++) {
//		cout << "Writing row " << i << " of " << height-1 << "." << endl;
		for (j = 0; j < width; j++) {
			output << choose_RGB(my_image[i][j], max_pixel) << " ";
		}
		output << endl;
	}
	output.close();
	cout << "Finished writing ppm file " << filename << "." << endl;
}

void image_to_raw(vector<vector<double> > my_image,
		string filename) {
	int height = my_image.size();
	int width = my_image[0].size();

	cout << "Writing raw data to " << filename << "." << endl;

	ofstream output;
	output.open(filename.c_str());

	int i,j;
	for (i = 0; i < height; i++) {
//		cout << "Writing row " << i << " of " << height-1 << "." << endl;
		for (j = 0; j < width; j++) {
//			cout << "(" << i << "," << j << "): " << my_image[i][j] << endl;
			output << my_image[i][j] << " ";
		}
		output << endl;
	}
	output.close();
	cout << "Finished writing to " << filename << "." << endl;
}

string choose_RGB(double pixel, double max_pixel) {
	/* Choose the color based on the value of the pixel, relative
	 * to the maximum value.
	 */
	int val;

	val = (int)ceil((pixel/max_pixel)*((RGB_SCALE*3)+2));

	string colorstr("");
	for (int i = 0; i < 3; i++) {
		if (val < 0) {
			colorstr.append("0");
		} else if (val > (RGB_SCALE)) {
			colorstr.append(int_to_string(RGB_SCALE));
		} else {
			colorstr.append(int_to_string(val));
		}
		val -= (RGB_SCALE+1);
		colorstr.append(" ");
	}
	return(colorstr);
}
