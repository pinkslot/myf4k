#ifndef GRIMSONGMM_H
#define GRIMSONGMM_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "GrimsonGMM_impl.h"

namespace alg
{

	class GrimsonGMM : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::GrimsonGMM bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "ggmm"; }
		inline string description() const { return "Grimson GMM"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		GrimsonGMM(void);
		~GrimsonGMM(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
