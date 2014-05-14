#ifndef EIGENBACKGROUND_H
#define EIGENBACKGROUND_H

#include <detection.h>
#include <cvblob.h>
#include <cv.h>
#include "Eigenbackground_impl.h"

namespace alg
{

	class Eigenbackground : public Detection
	{
		private:

		// Internal algorithm instance
		Algorithms::BackgroundSubtraction::Eigenbackground bgs;
		// Internal algorithm variables
		BwImage low_threshold_mask;
		BwImage high_threshold_mask;
		RgbImage frame_data;

		// Initialization flag
		bool init;
		
		public:
		
		inline string name() const { return "eigen"; }
		inline string description() const { return "Eigenbackground"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		Eigenbackground(void);
		~Eigenbackground(void){}

		cv::Mat nextFrame(Context& context);
	};

}

#endif
