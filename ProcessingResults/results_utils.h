// Utilities for handling processing results
#ifndef PROCESSING_RESULTS_UTILS
#define PROCESSING_RESULTS_UTILS

#include "results.h"
#include <cv.h>

namespace pr
{
	class ResultsUtils
	{
		public:
		
		// Get color associated to object ID
		static cv::Scalar objectColor(int id);

		// Filter by average size
		static Results filterByAverageSize(const Results& results, int threshold);
		
		// Filter by object size
		static Results filterBySize(const Results& results, int threshold);
		
		// Filter by number of appearances
		static Results filterByNumAppearances(const Results& results, int threshold, int upper_threshold = INT_MAX);

		// Filter by average detection certainty per object
		static Results filterByAverageDetectionCertainty(const Results& results, float threshold, bool add_by_default = true);
		
		// Filter by detection certainty per object
		static Results filterByDetectionCertainty(const Results& results, float threshold, bool add_by_default = true);
		
		// Filter by removing objects touching the border
		static Results filterByBorder(const Results& results, int height, int width);
			
		// Draw results on frame
		static void drawObjects(const Results& results, cv::Mat& frame, int frame_number, cv::Scalar color = CV_RGB(-1,-1,-1), int shadow_time = 0, int thickness = 1);
		static void drawTrajectories(const Results& results, cv::Mat& frame);
		static cv::Mat drawMask(const Results& results, int frame_number, int height=-1, int width=-1);

		// Draw method for single object/trajectory
		static void drawBoundingBox(const Object& object, cv::Mat& frame, int thickness = 1, cv::Scalar color = CV_RGB(-1,-1,-1));
		static void drawContour(const Object& object, cv::Mat& frame, int thickness = 1, cv::Scalar color = CV_RGB(-1,-1,-1));
		static void drawTrajectory(const ObjectList& objects, cv::Mat& frame, int from_frame = -1, int to_frame = -1, int thickness = 1, cv::Scalar color = CV_RGB(-1,-1,-1));
		static void drawSpecies(const Object& object, cv::Mat& frame, cv::Scalar color = CV_RGB(-1,-1,-1));

	};
}

#endif
