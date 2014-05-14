#include "blob_smoothing.h"
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <opencv_utils.h>
// Debug includes
#include <iostream>

using namespace std;
using namespace cv;

namespace alg
{

	BlobSmoothing::BlobSmoothing()
	{
		// Setup parameters
		parameters.add<float>("window", 3);
	}

	BlobSmoothing::~BlobSmoothing()
	{
	}


	void BlobSmoothing::filterBlobs(const Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const Mat* next_frame)
	{
		// Copy parameters to local variables
		float window = parameters.get<float>("window");
		// Redraw blobs
		Mat blob_image = rebuildBlobs(blobs, frame.cols, frame.rows);
		Mat filtered;
		medianBlur(blob_image, filtered, window);
		IplImage *out = cvCreateImage(cvSize(frame.cols, frame.rows), IPL_DEPTH_LABEL, 1);
		IplImage ipl_filtered = filtered;
		cvb::cvLabel(&ipl_filtered, out, filtered_blobs);
		cvReleaseImage(&out);
	}
		
}
