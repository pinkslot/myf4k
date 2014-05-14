// Class for the computation and application of Gabor filters
// TODO: indipendently manage each channel

#include "gabor_filter.h"
#include <my_exception.h>
#include <cmath>
#include <histogram.h>
// Debug includes
#include <highgui.h>
#include <iostream>
//using namespace std;
	
// Creates a real (i.e. using cos()) Gabor filter with the specified parameters.
// - lambda: wavelength
// - theta: orientation, in radians
// - psi: phase offset
// - bandwidth: bandwidth of the gaussian support (used to compute sigma_x and sigma_y)
// - gamma: aspect ratio of the gaussian support
Mat GaborFilter::createFilter(double lambda, double theta, double psi, double bandwidth, double gamma, int size)
{
	// Compute sigma
	double sigma = lambda/M_PI*sqrt(log(2)/2)*(pow(2,bandwidth)+1)/(pow(2,bandwidth)-1);
	// Compute Gabor filter size, depending on sigma, according to some Matlab script
	double sigma_x = sigma;
	double sigma_y = sigma/gamma;
	int theorical_size = (int) floor(8*(sigma_x >= sigma_y ? sigma_x : sigma_y));
	int g_size = (size > 0 ? (size < theorical_size ? size : theorical_size) : theorical_size);
	if(g_size % 2 == 0) g_size += 1;
	// Define Gabor filter matrix g(x,y)
	Mat g(g_size, g_size, CV_32F);
	// Compute each element of the filter, on a coordinate system where the central element is (0,0), x is positive to the right and y to the top
	int max_coord = (int) floor(g_size/2);
	for(int x=-max_coord; x<=max_coord; x++)
	{
		for(int y=-max_coord; y<=max_coord; y++)
		{
			// Compute rotated coordinates
			double x_ = x*cos(theta) + y*sin(theta);
			double y_ = -x*sin(theta) + y*cos(theta);
			// Compute matrix position
			int x_mat = x + max_coord;
			int y_mat = -y + max_coord;
			// Compute filter element
			g.at<float>(y_mat,x_mat) = exp(-0.5*(x_*x_/(sigma_x*sigma_x) + y_*y_/(sigma_y*sigma_y)))*cos(2*M_PI*x_/lambda + psi);
		}
	}
	// Return filter matrix
	return g;
}

// Applies a Gabor filter to an image and returns the filtered image
// - image: input image
// - filter_1: filter_1 is the real component of the Gabor filter, as obtained by createGaborFilter()
// - filter_2: filter_2 is the imaginary component of the Gabor filter, as obtained by createGaborFilter(); if not given, only the real part is used.
Mat GaborFilter::applyFilter(const Mat& image, const Mat& filter_real, const Mat& filter_imag)
{
	// Check input type
	if(image.type() != CV_8UC1)
	{
		throw MyException("Input to GaborFilter::applyFilter() must be a single-channel 8-bit image.");
	}
	// Check if we want to use the imaginary filter
	bool imag = filter_imag.rows > 0 && filter_imag.cols > 0;
	// Convert input image to CV_32F
	Mat image_fp;
	image.convertTo(image_fp, CV_32F);
	// Define filtered images
	Mat filter_real_out, filter_imag_out;
	// Apply real filter and, if necessary, imaginary filter to the input image
	filter2D(image_fp, filter_real_out, CV_32F, filter_real, Point(-1,-1), 0, BORDER_CONSTANT);
	if(imag) filter2D(image_fp, filter_imag_out, CV_32F, filter_imag, Point(-1,-1), 0, BORDER_CONSTANT);
	// Define the output image
	Mat filter_out;
	// Merge the real and imaginary component with L2-norm, if necessary, or just use the real part
	if(imag)
	{
		Mat f_real_sqr; multiply(filter_real_out, filter_real_out, f_real_sqr);
		Mat f_imag_sqr; multiply(filter_imag_out, filter_imag_out, f_imag_sqr);
		Mat f_sum_sqr = f_real_sqr + f_imag_sqr;
		sqrt(f_sum_sqr, filter_out);
	}
	else
	{
		filter_out = abs(filter_real_out);
	}
	// Get number of channels (for the computation of the maximum for each channel, for normalization)
	int channels = filter_out.channels();
	// Create color plane array
	Mat* filter_out_planes = new Mat[channels];
	// Split filter result into separate matrices, one for each channel
	split(filter_out, filter_out_planes);
	// Find the maximum for each plane and normalize it
	for(int i=0; i<channels; i++)
	{
		// Find maximum
		double plane_max;
		minMaxLoc(filter_out_planes[i], NULL, &plane_max);
		// Normalize plane
		filter_out_planes[i] = filter_out_planes[i]/plane_max;
	}
	// Join planes into a single multi-channel matrix
	Mat norm_out;
	merge(filter_out_planes, channels, norm_out);
	// Convert the output image to CV_8U
	Mat norm_out_8bit;
	norm_out.convertTo(norm_out_8bit, CV_8U, 255);
	// Free allocated memory
	delete [] filter_out_planes;
	// Return filtered image
	return norm_out_8bit;
}
	
// Applies a set of Gabor filters (with both real and imaginary parts), given a range of orientations and scales, and returns a vector
// containing a list mean and variance for each output image
vector<float> GaborFilter::applyFilterSet(const Mat& image, float min_lambda, float max_lambda, int num_scales, int num_orientations, bool with_std_devs, float bandwidth, float gamma, int size)
{
	// Compute list of scales
	vector<float> lambda_list;
	for(int s=0; s<num_scales; s++)
	{
		// Compute corresponding scale
		float lambda = min_lambda + (s > 0 ? s*(max_lambda-min_lambda)/(num_scales-1) : 0);
		// Add to vector
		lambda_list.push_back(lambda);
	}
	// Compute result
	return applyFilterSet(image, lambda_list, num_orientations, with_std_devs, bandwidth, gamma, size);
}
	
vector<float> GaborFilter::applyFilterSet(const Mat& image, const vector<float>& lambda_list, int num_orientations, bool with_std_devs, float bandwidth, float gamma, int size)
{
	// Define result vector
	vector<float> result;
	// Apply a Gabor filter for each orientation/scale combination 
	for(vector<float>::const_iterator s_it = lambda_list.begin(); s_it != lambda_list.end(); s_it++)
	{
		for(int o=0; o<num_orientations; o++)
		{
			// Compute corresponding angle
			float theta = o*M_PI/num_orientations;
			// Compute corresponding scale
			float lambda = *s_it;
			// Create filters
			Mat g_real = GaborFilter::createFilter(lambda, theta, 0, bandwidth, gamma, size);
			Mat g_imag = GaborFilter::createFilter(lambda, theta, M_PI/2, bandwidth, gamma, size);
			// Apply filters
			Mat gabor_gs = GaborFilter::applyFilter(image, g_real, g_imag);
			// Compute mean and standard deviation
			Scalar mean, std_dev;
			meanStdDev(gabor_gs, mean, std_dev);
			// Add mean (and std dev, if required) to results
			result.push_back(mean[0]);
			if(with_std_devs)
				result.push_back(std_dev[0]);
		}
	}
	// Return result
	return result;
}
