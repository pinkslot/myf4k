#ifndef ALG_BLOB_SIZE_FILTER_H
#define ALG_BLOB_SIZE_FILTER_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "blob_processing.h"

using namespace std;

namespace alg
{

	class BlobSizeFilter : public BlobProcessing
	{
		public:
		
		//Methods
		BlobSizeFilter();
		~BlobSizeFilter();
		
		void filterBlobs(const cv::Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const cv::Mat* next_frame = NULL);

		// Implement algorithm information
		inline string name() const { return "blob_size_filter"; }
		inline string description() const { return "Blob size filtering"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
