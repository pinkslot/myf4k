#include "blob_certainty_filter.h"
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <boost/thread.hpp>
#include <bayes_detection_score.h>
#include <bayes_detection_evaluator.h>
// Debug includes
#include <iostream>

using namespace std;
using namespace cv;

namespace alg
{

	BlobCertaintyFilter::BlobCertaintyFilter() :
		init(true)
	{
		// Setup parameters
		parameters.add<string>("evaluator", "bayes_det_eval");
		parameters.add<float>("threshold", 0.5);
	}

	BlobCertaintyFilter::~BlobCertaintyFilter()
	{
		//cvReleaseImage(&labelImg);
	}


	void BlobCertaintyFilter::filterBlobs(const Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const Mat* next_frame)
	{
		// Check initialization
		if(init)
		{
			init = false;
			// Create evaluator
			evaluator = DetectionEvaluationChooser::create(parameters.get<string>("evaluator"));
		}
		// Clear output variable
		filtered_blobs.clear();
		// Define thread group
		boost::thread_group threads;
		// Map between blob pointers and score pointers
		map<cvb::CvBlob*, BayesDetectionScore*> blob_score_map;
		// Go through all input blobs
		for(cvb::CvBlobs::const_iterator b_it = blobs.begin(); b_it != blobs.end(); b_it++)
		{
			// Get blob reference
			const cvb::CvBlob* blob = b_it->second;
			// Clone blob for map
			cvb::CvBlob* map_copy = new cvb::CvBlob();
			cvCloneBlob(*blob, *map_copy);
			isBlobValid(*map_copy);
			// Compute score
			BayesDetectionScore* score = new BayesDetectionScore(&nbc);
			// Add to map
			blob_score_map[map_copy] = score;
			// Clone blob for thread
			cvb::CvBlob* thread_copy = new cvb::CvBlob();
			cvCloneBlob(*blob, *thread_copy);
			// Run thread
			boost::thread* thread = new boost::thread(boost::bind(&BayesDetectionEvaluator::computeScoreThread, (BayesDetectionEvaluator*) evaluator, _1, _2, _3, _4), boost::ref(*thread_copy), boost::ref(frame), boost::ref(*score), next_frame);
			threads.add_thread(thread);
		}
		// Wait for all threads
		threads.join_all();	
		// Assign scores to the corresponding blobs, and check for certainty threshold
		int label_id = 1;
		for(map<cvb::CvBlob*, BayesDetectionScore*>::iterator it = blob_score_map.begin(); it != blob_score_map.end(); ++it)
		{
			// Get pointers to blob and score
			cvb::CvBlob* blob = it->first;
			//showBlob("b", *blob);
			//waitKey(0);
			BayesDetectionScore *score = it->second;
			// Assign certainty to score
			blob->detection_certainty = score->value();
			Log::d() << "Blob certainty: " << score->value() << endl;
			// Check certainty
			if(blob->detection_certainty >= parameters.get<float>("threshold"))
			{
				// Add to output
				filtered_blobs[label_id++] = blob;
			}
		}
	}
		
}
