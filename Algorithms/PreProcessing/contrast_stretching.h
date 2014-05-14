// Contrast-stretching based image preprocessing

#ifndef CONTRAST_STRETCHING_H
#define CONTRAST_STRETCHING_H

#include "preprocessing.h"
#include <histogram.h>

namespace alg
{

	class ContrastStretching : public PreProcessing
	{
		private:

		// Filter out all bins at the extremities which are below the given threshold
		void trimHist(const Mat& image, float threshold, int& min_bin, int& max_bin);

		public:
		
		virtual string name() const { return "contrast_stretch"; }
		virtual string description() const { return "Histogram trimming and contrast stretching"; }
		virtual string version() const { return "1.0"; }
		virtual int executionTime() const { return 0; }
		virtual int ram() const { return 0; }

		// Constructor; for parameter definition/initialization
		ContrastStretching();

		// Apply contrast stretching to current frame
		// (Assuming BGR color)
		Mat nextFrame(const Mat& frame);
	};

}

#endif
