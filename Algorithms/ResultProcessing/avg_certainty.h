#ifndef ALG_AVG_CERTAINTY_H
#define ALG_AVG_CERTAINTY_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "result_processing.h"

namespace alg
{

	class AvgCertainty : public ResultProcessing
	{
		public:
		
		//Methods
		AvgCertainty();
		~AvgCertainty();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "avg_certainty"; }
		inline std::string description() const { return "Average certainty filter"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
