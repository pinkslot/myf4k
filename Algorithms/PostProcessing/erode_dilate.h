#ifndef ALG_ERODE_DILATE_H
#define ALG_ERODE_DILATE_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "postprocessing.h"

namespace alg
{

	class ErodeDilate : public PostProcessing
	{
		public:
		
		//Methods
		ErodeDilate();
		~ErodeDilate();
		
		cv::Mat nextFrame(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "erode_dilate"; }
		inline std::string description() const { return "Erosion/dilation"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
