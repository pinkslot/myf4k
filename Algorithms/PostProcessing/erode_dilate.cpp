#include "erode_dilate.h"
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
// Debug includes
#include <iostream>

using namespace std;
using namespace cv;

namespace alg
{

	ErodeDilate::ErodeDilate()
	{
		// Setup parameters
		parameters.add<int>("cycles", 3);
	}

	ErodeDilate::~ErodeDilate()
	{
	}

	Mat ErodeDilate::nextFrame(Context& context)
	{
		// Copy parameters to local variables
		int cycles = parameters.get<int>("cycles");
		// Process
		Mat copy = context.motion_output.clone();
		erode(copy, copy, Mat(), Point(-1, -1), cycles);
		dilate(copy, copy, Mat(), Point(-1, -1), cycles);
		// Set result and return
		context.postproc_output = copy;
		return copy;
	}
		
}
