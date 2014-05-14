// Implementation of a detection evaluator inspired Erdem's based

#ifndef BAYES_DETECTION_EVALUATOR_H
#define BAYES_DETECTION_EVALUATOR_H

#include <map>
#include <vector>
#include <cv.h>
#include <cvblob.h>
#include <evaluator.h>
#include <histogram.h>
#include <detection_bayes_classifier.h>
//#include <boost/thread.hpp>
#include <threadpool.hpp>
#include "bayes_detection_score.h"

namespace alg
{

	class BayesDetectionEvaluator : public Evaluator
	{
		private:

		// Thread pool
		boost::threadpool::pool thread_pool;

		// Mutexes
		boost::mutex objects_mutex;
		boost::mutex out_file_mutex;
		boost::mutex log_mutex;

		// Train data file
		std::ofstream out_file;

		// Naive Bayes classifier, for score combination - specifically trained for detection
		DetectionBayesClassifier nbc;

		// Gabor scales
		std::vector<float> gabor_scales;

		// Compute the color_contrast_score value for an object
		void computeColorContrastScore(const cv::Mat& frame, const cvb::CvBlob& blob, float* result);
		
		// Compute the edge density scpre for an object
		void computeEdgeDensityScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result);

		// Compute the superpixels straddling score
		void computeSuperpixelsStraddlingScore(const cv::Mat& sp_seg, const cvb::CvBlob& blob, float& result);

		// Compute the Gestalt-based score
		void computeGestaltScore(const cvb::CvBlob& blob, int rows, int cols, float& result);
		
		// Compute the color_internal_score value for an object
		void computeColorInternalScore(const cv::Mat& frame, const cvb::CvBlob& blob, float* result);
		
		// Compute the motion_boundary_score value for an object
		void computeMotionBoundaryScore(const cv::Mat& motion_vector, const cvb::CvBlob& blob, float* result);

		// Compute the motion internal score value for an object
		void computeMotionInternalScore(const cv::Mat& motion_vector, const cvb::CvBlob& blob, const cv::Rect& search_area, float* result);
		
		// Compute the internal texture score value for an object
		// Returns -1 if there are not enough cells on which to apply the Gabor filter
		void computeTextureScore(const cv::Mat& frame, const cvb::CvBlob& blob, const cv::Mat& mask, float& result);
		
		public:
		
		// Threads for computing an object's score
		//void computeScoreThread(const TrackedObject& tracked_object, const Mat& frame, int frame_number);
		// BLOB IS DELETED AT THE END OF THE METHOD
		void computeScoreThread(const cvb::CvBlob& blob, const cv::Mat& frame, BayesDetectionScore& score, const cv::Mat* next_frame = NULL);
		
		// Information on the algorithm - must be filled in by derived class
		inline string name() const { return "bayes_det_eval"; }
		inline string type() const { return "detection_evaluation"; }
		inline string description() const { return "Weighted combination of motion/color/texture features"; }
		inline string version() const { return "1.1"; }
		inline int executionTime() const { return 0; }
		inline string descriptionExecutionTime() const { return "Average time per detection (blob)"; }
		inline int ram() const { return 0; }

		// Override settings() to handle vector parameters
		//string settings() const;
		
		// Constructor:
		// - frame_buffer_size: number of previous frames to be stored, in order to compare them with objects recognized in the current frame but which have been missing for a while
		BayesDetectionEvaluator(int frame_buffer_size = 5);


		// Given the list of currently tracked object, perform tracking evaluation on the current frame.
		// If training_class > -1, print training data to file
		//inline void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number) { addFrame(tracked_objects, frame, frame_number, NULL); }
		void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number, const cv::Mat* next_frame);

		// Create a new tracked object score and add it to the list:
		// - object_id: tracked object (as managed by the tracker) ID
		//void addObject(int object_id);

		// Set the score for an object on a specific frame:
		// - object_id
		// - frame: number of frame to which this score is related
		// - score: evaluated score
		//void setScore(int object_id, int frame, float score);

	};

}

#endif
