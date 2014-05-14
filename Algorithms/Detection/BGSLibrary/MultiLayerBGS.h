#ifndef ALG_MULTI_LAYER_BGS_H
#define ALG_MULTI_LAYER_BGS_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "MultiLayerBGS_impl.h"

namespace alg
{

	class MultiLayerBGS : public Detection
	{
		private:

		// Internal algorithm instance
		MultiLayerBGS_impl bgs;
		// Internal algorithm variables
		//BwImage low_threshold_mask;
		//BwImage high_threshold_mask;
		//RgbImage frame_data;

		// Initialization flag
		//bool init;
		
		public:
		
		inline string name() const { return "ml"; }
		inline string description() const { return "MultiLayer"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		MultiLayerBGS(void);
		//~MultiLayerBGS(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
