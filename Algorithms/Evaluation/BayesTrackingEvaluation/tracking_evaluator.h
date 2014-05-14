// Implementation of a tracking evaluator

#ifndef TRACKING_EVALUATOR_H
#define TRACKING_EVALUATOR_H

#include <map>
#include <vector>
#include <cv.h>
#include <cvblob.h>
#include <evaluator.h>
#include <histogram.h>
#include "tracking_score.h"
#include "tracking_score_history.h"
#include <fstream>
#include "tracking_bayes_classifier.h"

// Short name for a sampled contour
//typedef vector<pair<Point, Point> > SampledContour;

namespace alg
{

	class TrackingEvaluator : public Evaluator
	{
		private:

		// Train data file
		std::ofstream out_file;

		// Naive Bayes classifier, used to combine the single scores
		TrackingBayesClassifier nbc;

		// Parameter: Gabor scales
		vector<float> gabor_scales;

		// Compute single score
		TrackingScore* computeScore(const alg::TrackedObject& tracked_object, const cv::Mat& frame, int frame_number, TrackingScoreHistory *history);

		// Compute the shape ratio score for an object
		float computeShapeRatioScore(float sr_1, float sr_2);//const Mat& frame, const SampledContour& sampled_contour);
		
		// Compute the area score for an object
		float computeAreaScore(float sr_1, float sr_2);//const Mat& frame, const SampledContour& sampled_contour);
		
		// Compute the temporal score for an object
		float computeTemporalScore();//const Histogram& hist_1, const Histogram& hist_2);
		
		// Compute the histogram distance score for an object
		float computeHistogramDiffScore(const TrackingScore* score_1, const TrackingScore* score_2);

		// Compute the motion smoothness score value for an object
		float computeMotionSmoothnessScore(float ms_1, float ms_2);//const Mat& motion_vector, const cvb::CvBlob& blob, const Mat& mask, const Rect& search_area);
		
		// Compute the direction score value for an object
		float computeDirectionScore(float d_1, float d_2);//const Mat& frame, const cvb::CvBlob& blob, const Mat& mask);
		
		// Compute the texture difference score value for an object
		float computeTextureDiffScore(vector<float> tf_1, vector<float> tf_2);//const Mat& frame, const cvb::CvBlob& blob, const Mat& mask);
		
		// Perform contour sampling, for each sample draw a normal segment with length "l" and get the two ends of each segment
		//SampledContour sampleContour(const cvb::CvBlob& blob);//, Mat& drawn);
		
		public:
			
		// Information on the algorithm - must be filled in by derived class
		inline string name() const { return "bayes_track_eval"; }
		inline string type() const { return "tracking_evaluation"; }
		inline string description() const { return "Bayesian combination of motion/color/texture features"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline string descriptionExecutionTime() const { return "Average time per tracking decision"; }
		inline int ram() const { return 0; }

		// Override settings() to handle vector<float> parameter
		string settings() const;
		
		// Constructor:
		TrackingEvaluator();

		// Given the list of currently tracked object, perform tracking evaluation on the current frame
		// - tracked_objects: list of tracked objects
		// - frame: frame matrix
		// - frame_num: frame number
		virtual void addFrame(const vector<const alg::TrackedObject*>& tracked_objects, const cv::Mat& frame, int frame_number, const cv::Mat* next_frame = NULL);

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
