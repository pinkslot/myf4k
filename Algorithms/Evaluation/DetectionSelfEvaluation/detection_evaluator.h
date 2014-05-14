// Implementation of a detection evaluator inspired Erdem's based

#ifndef DETECTION_EVALUATOR_H
#define DETECTION_EVALUATOR_H

#include <map>
#include <vector>
#include <cv.h>
#include <cvblob.h>
#include <evaluator.h>
#include <histogram.h>

namespace alg
{

	// Short name for a sampled contour
	typedef vector<pair<Point, Point> > SampledContour;

	class DetectionEvaluator : public Evaluator
	{
		private:

		// Compute the color_boundary_score value for an object
		float computeColorBoundaryScore(const cv::Mat& frame, const SampledContour& sampled_contour);
		
		// Compute the color_internal_score value for an object
		float computeColorInternalScore(const cv::Mat& frame, const cvb::CvBlob& blob, const cv::Mat& mask);
		
		// Compute the motion_boundary_score value for an object
		float computeMotionBoundaryScore(const cv::Mat& motion_vector, const SampledContour& sampled_contour);

		// Compute the motion internal score value for an object
		float computeMotionInternalScore(const cv::Mat& motion_vector, const cvb::CvBlob& blob, const cv::Mat& mask, const cv::Rect& search_area);
		
		// Compute the internal texture score value for an object
		// Returns -1 if there are not enough cells on which to apply the Gabor filter
		float computeTextureScore(const cv::Mat& frame, const cvb::CvBlob& blob, const cv::Mat& mask);
		
		// Perform contour sampling, for each sample draw a normal segment with length "l" and get the two ends of each segment
		SampledContour sampleContour(const cvb::CvBlob& blob);//, Mat& drawn);
		
		public:
		
		// Information on the algorithm - must be filled in by derived class
		inline string name() const { return "simple_det_eval"; }
		inline string type() const { return "detection_evaluation"; }
		inline string description() const { return "Weighted combination of motion/color/texture features"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline string descriptionExecutionTime() const { return "Average time per detection (blob)"; }
		inline int ram() const { return 0; }
		
		// Constructor:
		// - frame_buffer_size: number of previous frames to be stored, in order to compare them with objects recognized in the current frame but which have been missing for a while
		DetectionEvaluator(int frame_buffer_size = 5);

		// Given the list of currently tracked object, perform tracking evaluation on the current frame
		// - tracked_objects: list of tracked objects
		// - frame: frame matrix
		// - frame_num: frame number
		virtual void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number);

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
