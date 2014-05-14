#ifndef PRATIMEDIODBGS_H
#define PRATIMEDIODBGS_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "PratiMediodBGS_impl.h"

namespace alg
{

	class PratiMediod : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::PratiMediodBGS bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "pm"; }
		inline string description() const { return "PratiMediod"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		PratiMediod(void);
		~PratiMediod(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
