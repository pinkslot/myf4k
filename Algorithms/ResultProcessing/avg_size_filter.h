#ifndef ALG_AVG_SIZE_FILTER_H
#define ALG_AVG_SIZE_FILTER_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "result_processing.h"

namespace alg
{

	class AvgSizeFilter : public ResultProcessing
	{
		public:
		
		//Methods
		AvgSizeFilter();
		~AvgSizeFilter();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "avg_size_filter"; }
		inline std::string description() const { return "Object average size filter"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
