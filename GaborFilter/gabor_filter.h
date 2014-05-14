// Class for the computation and application of Gabor filters

#ifndef GABOR_FILTER_H
#define GABOR_FILTER_H

#include <cv.h>

using namespace cv;

class GaborFilter
{
	public:

	// Creates a real (i.e. using cos()) Gabor filter with the specified parameters.
	// - lambda: wavelength
	// - theta: orientation, in radians
	// - psi: phase offset
	// - bandwidth: bandwidth of the gaussian support (used to compute sigma_x and sigma_y)
	// - gamma: aspect ratio of the gaussian support
	// - size: filter size, or -1 to compute it automatically
	static Mat createFilter(double lambda, double theta, double psi=0, double bandwidth=1, double gamma=0.5, int size=-1);

	// Applies a Gabor filter to an image and returns the filtered image
	// - image: input image (CV_8UC1)
	// - filter_1: filter_1 is the real component of the Gabor filter, as obtained by createGaborFilter()
	// - filter_2: filter_2 is the imaginary component of the Gabor filter, as obtained by createGaborFilter(); if not given, only the real part is used.
	static Mat applyFilter(const Mat& image, const Mat& filter_real, const Mat& filter_imag = Mat(0,0,CV_8U));

	// Applies a set of Gabor filters (with both real and imaginary parts), given a range of orientations and scales, and returns a vector
	// containing a list mean (and standard deviation, if required) for each output image
	static vector<float> applyFilterSet(const Mat& image, float min_lambda, float max_lambda, int num_scales, int num_orientations, bool with_std_devs = true, float bandwidth=1.0f, float gamma=0.5f, int size=-1);
	static vector<float> applyFilterSet(const Mat& image, const vector<float>& lambda_list, int num_orientations, bool with_std_devs = true, float bandwidth=1.0f, float gamma=0.5f, int size=-1);
};

#endif
