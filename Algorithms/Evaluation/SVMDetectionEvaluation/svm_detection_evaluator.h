#ifndef SVM_DETECTION_EVALUATOR_H
#define SVM_DETECTION_EVALUATOR_H

#include <map>
#include <vector>
#include <cv.h>
#include <cvblob.h>
#include <evaluator.h>
#include <histogram.h>
#include <svm_utils.h>
//#include <boost/thread.hpp>
#include <threadpool.hpp>
#include "svm_detection_score.h"

namespace alg
{

	class SVMDetectionEvaluator : public Evaluator
	{
		private:
		public:

		// Thread pool
		boost::threadpool::pool thread_pool;

		// Mutexes
		boost::mutex objects_mutex;
		boost::mutex out_file_mutex;
		boost::mutex log_mutex;

		// Train data file
		std::ofstream out_file;

		// SVM classifier
		SVMUtils svm;
		
		// Compute the boundary complexity score
		void computeBoundaryComplexityScore(const cvb::CvBlob& blob, int rows, int cols, float& result);

		// Compute the color_contrast_score value for an object
		void computeColorContrastScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result);
		
		// Compute the edge density scpre for an object
		//void computeEdgeDensityScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result);

		// Compute the superpixels straddling score
		void computeSuperpixelsStraddlingScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result);

		// Compute the motion_boundary_score value for an object
		void computeMotionBoundaryScore(const cv::Mat& frame, const Mat* next_frame, const cvb::CvBlob& blob, float& result);
		
		// Compute the motion internal score value for an object
		void computeMotionInternalScore(const cv::Mat& frame, const Mat* next_frame, const cvb::CvBlob& blob, float& result);
		
		// Compute the color internal score value for an object
		void computeColorInternalScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result);
		
		public:
		
		// Threads for computing an object's score
		//void computeScoreThread(const TrackedObject& tracked_object, const Mat& frame, int frame_number);
		// BLOB IS DELETED AT THE END OF THE METHOD
		void computeScoreThread(const cvb::CvBlob& blob, const cv::Mat& frame, SVMDetectionScore& score, const cv::Mat* next_frame = NULL);
		
		// Information on the algorithm - must be filled in by derived class
		inline string name() const { return "svm_det_eval"; }
		inline string type() const { return "detection_evaluation"; }
		inline string description() const { return "SVM detection certainty evaluator"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline string descriptionExecutionTime() const { return "Average time per detection (blob)"; }
		inline int ram() const { return 0; }

		// Constructor:
		// - frame_buffer_size: number of previous frames to be stored, in order to compare them with objects recognized in the current frame but which have been missing for a while
		SVMDetectionEvaluator(int frame_buffer_size = 5);


		// Given the list of currently tracked object, perform tracking evaluation on the current frame.
		// If training_class > -1, print training data to file
		//inline void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number) { addFrame(tracked_objects, frame, frame_number, NULL); }
		void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number, const cv::Mat* next_frame);

	};

}

#endif
