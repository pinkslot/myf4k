#ifndef MEANBGS_H
#define MEANBGS_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "MeanBGS_impl.h"

namespace alg
{

	class MeanBGS : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::MeanBGS bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "mean"; }
		inline string description() const { return "Mean subtractor"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		MeanBGS(void);
		~MeanBGS(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
