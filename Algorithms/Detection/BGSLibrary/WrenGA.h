#ifndef WRENGA_H
#define WRENGA_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "WrenGA_impl.h"

namespace alg
{

	class WrenGA : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::WrenGA bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "wren"; }
		inline string description() const { return "WrenGA"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		WrenGA(void);
		~WrenGA(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
