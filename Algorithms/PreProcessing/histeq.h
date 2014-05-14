#ifndef HISTEQ_H
#define HISTEQ_H

#include "preprocessing.h"
#include <histogram.h>

namespace alg
{

	class HistEq : public PreProcessing
	{
		public:
		
		virtual string name() const { return "histeq"; }
		virtual string description() const { return "Histogram equalization"; }
		virtual string version() const { return "1.0"; }
		virtual int executionTime() const { return 0; }
		virtual int ram() const { return 0; }

		// Constructor; for parameter definition/initialization
		HistEq();

		// Apply contrast stretching to current frame
		// (Assuming BGR color)
		Mat nextFrame(const Mat& frame);
	};

}

#endif
