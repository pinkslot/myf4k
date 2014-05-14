#ifndef ALG_BLOB_CERTAINTY_FILTER_H
#define ALG_BLOB_CERTAINTY_FILTER_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "blob_processing.h"
#include <detection_evaluation_chooser.h>
#include <detection_bayes_classifier.h>

using namespace std;

namespace alg
{

	class BlobCertaintyFilter : public BlobProcessing
	{
		private:

		bool init;
		Evaluator* evaluator;
		DetectionBayesClassifier nbc;

		public:
		
		//Methods
		BlobCertaintyFilter();
		~BlobCertaintyFilter();
		
		void filterBlobs(const cv::Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const cv::Mat* next_frame = NULL);

		// Implement algorithm information
		inline string name() const { return "blob_certainty_filter"; }
		inline string description() const { return "Blob certainty filtering"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
