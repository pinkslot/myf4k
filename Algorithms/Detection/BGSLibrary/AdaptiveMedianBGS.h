#ifndef ADAPTIVE_MEDIAN_H
#define ADAPTIVE_MEDIAN_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "AdaptiveMedianBGS_impl.h"

namespace alg
{

	class AdaptiveMedian : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::AdaptiveMedianBGS bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "am"; }
		inline string description() const { return "Adaptive Median"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		AdaptiveMedian(void);
		~AdaptiveMedian(void);

		cv::Mat nextFrame(Context& context);
	};

}

#endif
