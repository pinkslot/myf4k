#ifndef ALG_BLOB_SMOOTHING_H
#define ALG_BLOB_SMOOTHING_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "blob_processing.h"

using namespace std;

namespace alg
{

	class BlobSmoothing : public BlobProcessing
	{
		public:
		
		IplImage *input_frame;// = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 3);
		//	unsigned int result = cvLabel(processed_frame, labelImg, blobs);
		/*float area_min_per;
		float area_max_per;
		int offset;
		float elongation_min;
		float elongation_max;
		float compactness;*/
		//IplImage* labelImg;
		
		//Methods
		BlobSmoothing();
		~BlobSmoothing();
		
		void filterBlobs(const cv::Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const cv::Mat* next_frame);

		// Implement algorithm information
		inline string name() const { return "blob_smoothing"; }
		inline string description() const { return "Blob median smoothing"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
