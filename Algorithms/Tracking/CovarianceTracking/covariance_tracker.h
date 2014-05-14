// Class for covariance tracking.
// Naming conventions:
// - W: image width
// - H: image height
// - d: number of features

#ifndef COVARIANCE_TRACKER_H
#define COVARIANCE_TRACKER_H

#include <cv.h>
#include <vector>
#include "covariance_feature_extractor.h"
#include "feature_extractor_set.h"
#include "xy_feature_extractor.h"
#include "rgb_feature_extractor.h"
#include "hue_feature_extractor.h"
#include "hist_feature_extractor.h"
#include "deriv_feature_extractor.h"
#include "covariance_tracked_object.h"
#include "covariance_detected_object.h"
#include "../tracked_object.h"
#include "../tracker.h"
#include <log.h>

namespace alg
{

	class CovarianceTracker : public Tracker
	{
		private:

		// Define a tracked_object/detected_object couple (see covariance_tracker.cpp)
		typedef std::pair<CovarianceTrackedObject*, CovarianceDetectedObject*> TDPair;
		// Define a TDPair/distance couple (see covariance_tracker.cpp)
		typedef std::pair<TDPair, float> TDCandidate;
		
		// Function for comparing two TDCandidate objects (see covariance_tracker.cpp)
		inline static bool compareCandidates(const TDCandidate& c1, const TDCandidate& c2) { return c1.second < c2.second; }

		// Quantize area
		int quantizeArea(int area);

		// Compute search area for tracked object
		int computeSearchArea(const CovarianceTrackedObject& to);
	
		// Get search regions
		void computeSearchRegions(const CovarianceTrackedObject& tracked_object, Rect& t_region, Rect& dir_region);

		// Variables for keeping track of the average search area by blob size
		std::map<int, int> num_samples_per_size;
		std::map<int, int> sum_distance_per_size;
		
		protected:

		// Utility method for computing the center of a region
		inline cv::Point2i center(const cv::Rect& rect) { return cv::Point2i(rect.x + rect.width/2, rect.y + rect.height/2); }
		
		public:

		inline std::string name() const { return "cov"; }
		inline std::string description() const { return "Covariance tracker"; }
		inline std::string version() const { return "2.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		// Drawing options
		bool drawSearchAreaFlag;
		
		// Default constructor
		CovarianceTracker();
		
		// Destructor; has to free tracked objects
		~CovarianceTracker();

		// Returns the number of currently tracked objects
		//inline virtual int numTrackedObjects() { return tracked_objects.size(); }

		// Instance of FeatureExtractor to be used
		CovarianceFeatureExtractor* featureExtractor;

		// Find object (i.e. square region containing object) in given area
		cv::Rect findObject(const cv::Mat& model, const cv::Mat& frame, const cv::Rect& area, int min_size, int max_size, int step = 1) const;
		
		// Read next frame, frame number, object blobs and perform tracking.
		// The input image type is supposed to be CV_8UC3.
		void nextFrame(const cv::Mat&, int, const cvb::CvBlobs& blobs);
		
		// Adapt tracking parameters to video properties
		void adaptToVideo(cv::VideoCapture& video_cap);

		// Draw objects' tracks.
		// The input matrix should be the frame on which to draw.
		void drawTracks(cv::Mat&);
	};

}

#endif
