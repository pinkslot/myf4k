// Represents the detection score for a certain object in a frame;
// Also contains information necessary to compute values at a later time (e.g. histogram vector and motion vector)

#ifndef DETECTION_SCORE_H
#define DETECTION_SCORE_H

#include <vector>
#include <cv.h>
#include <score.h>
#include <histogram.h>

namespace alg
{

	class DetectionScore : public Score
	{
		private:

		// Histogram vector for the frame to which this object refers
		Histogram histogram;

		// Motion vector between the previous frame and the one this object refers to
		cv::Mat motion_vector;

		// Sampled contour (with the ends of the normal segment for each sample)
		std::vector<std::pair<cv::Point, cv::Point> > sampled_contour;

		public:

		// Measure of color differences on boundary
		float color_boundary_score;
		// Measure of motion vector differences on boundary
		float motion_boundary_score;
		// Measure of motion vector differences inside the object
		float motion_internal_score;
		// Measure of color differences inside the object
		float color_internal_score;
		// Measure of texture differences inside the object
		float texture_diff_score;
		
		// Weights used for each value
		float w_cb, w_mb, w_mi, w_ci, w_td;

		// Constructor; initializes everything to zero
		DetectionScore() :	color_boundary_score(0), motion_boundary_score(0), motion_internal_score(0), color_internal_score(0), texture_diff_score(0),
					w_cb(0), w_mb(0), w_mi(0), w_ci(0), w_td(0) {}

		// Save histogram
		inline void setHistogram(Histogram h) { histogram = h; }
		
		// Return histogram
		inline Histogram getHistogram() { return histogram; }
		
		// Save motion vector
		//inline void setMotionVector(Mat mv) { motion_vector = mv; }
		
		// Return motion vector
		//inline Mat getMotionVector() { return motion_vector; }

		// Return motion vectors
		//inline vector<Point> getMotionVectors() { return motion_vectors; }
		
		// Set weights
		void setWeights(float weight_cb, float weight_mb, float weight_mi, float weight_ci, float weight_td)
		{
			w_cb = weight_cb;
			w_mb = weight_mb;
			w_mi = weight_mi;
			w_ci = weight_ci;
			w_td = weight_td;
		}

		// Set sampled contour
		inline void setSampledContour(std::vector<std::pair<cv::Point,cv::Point> > sc) { sampled_contour = sc; }

		// Return sampled contour
		inline std::vector<std::pair<cv::Point, cv::Point> > getSampledContour() { return sampled_contour; }

		// Compute single score
		virtual inline float value() { return 0.95f*(color_boundary_score*w_cb + motion_boundary_score*w_mb + motion_internal_score*w_mi + color_internal_score*w_ci + texture_diff_score*w_td)/(w_cb+w_mb+w_mi+w_td+w_ci); }
		
	};

}

#endif
