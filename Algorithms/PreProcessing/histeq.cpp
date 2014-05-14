#include "histeq.h"
#include <iostream>
#include <highgui.h>

using namespace cv;
using namespace std;

// Constructor; initialize parameters
alg::HistEq::HistEq()
{
}

// Apply histogram equalization to current frame
// (Assuming BGR color)
Mat alg::HistEq::nextFrame(const Mat& frame)
{
	// Split the image into the three channels
	Mat* channels = new Mat[3];
	split(frame, channels);
	// Equalize them
	Mat eq_b, eq_g, eq_r;
	equalizeHist(channels[0], eq_b);
	equalizeHist(channels[1], eq_g);
	equalizeHist(channels[2], eq_r);
	// Merge channels
	Mat* eq_channels = new Mat[3];
	eq_channels[0] = eq_b;
	eq_channels[1] = eq_g;
	eq_channels[2] = eq_r;
	Mat eq_frame;
	merge(eq_channels, 3, eq_frame);
	// Free matrices
	delete [] channels;
	delete [] eq_channels;
	// Return result
	return eq_frame;
}
