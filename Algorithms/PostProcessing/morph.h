#ifndef ALG_MORPH_H
#define ALG_MORPH_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "postprocessing.h"

namespace alg
{

	class Morph : public PostProcessing
	{
		public:
		
		//Methods
		Morph();
		~Morph();
		
		cv::Mat nextFrame(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "morph"; }
		inline std::string description() const { return "Morphological operation"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
