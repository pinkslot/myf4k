#ifndef ZIVKOVICAGMM_H
#define ZIVKOVICAGMM_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "ZivkovicAGMM_impl.h"

namespace alg
{

	class ZivkovicAGMM : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::ZivkovicAGMM bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "zgmm"; }
		inline string description() const { return "Zivkovic AGMM"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		ZivkovicAGMM(void);
		~ZivkovicAGMM(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
