#include "blob_size_filter.h"
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
// Debug includes
#include <iostream>

using namespace std;
using namespace cv;

namespace alg
{

	BlobSizeFilter::BlobSizeFilter()
	{
		// Setup parameters
		parameters.add<float>("area_min_per", 0.001);
		parameters.add<float>("area_max_per", 0.25);
		//parameters.add<float>("offset", 6);
		//parameters.add<float>("elongation_min", 0);
		//parameters.add<float>("elongation_max", 5);
		//parameters.add<float>("compactness", 35);
	}

	BlobSizeFilter::~BlobSizeFilter()
	{
		//cvReleaseImage(&labelImg);
	}


	void BlobSizeFilter::filterBlobs(const Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const Mat* next_frame)
	{
		// Copy parameters to local variables
		float area_min_per = parameters.get<float>("area_min_per");
		float area_max_per = parameters.get<float>("area_max_per");
		int area_min = (int)(frame.rows*frame.cols*area_min_per);
		int area_max = (int)(frame.rows*frame.cols*area_max_per);
		// Filter
		cvb::cvCloneBlobs(blobs, filtered_blobs);
		cvb::cvFilterByArea(filtered_blobs, area_min, area_max);
	}
		
}
