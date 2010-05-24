#ifndef IMAGE_HPP_
#define IMAGE_HPP_

#include <vector>
using namespace std;

void image_to_ppm(vector<vector<double> > image, string filename, string desc);
void image_to_raw(vector<vector<double> > image, string filename);
string choose_RGB(double pixel, double max_pixel);
string inttostring(int n);

#endif /* IMAGE_HPP_ */
