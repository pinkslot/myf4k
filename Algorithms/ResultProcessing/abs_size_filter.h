#ifndef ALG_ABS_SIZE_FILTER_H
#define ALG_ABS_SIZE_FILTER_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "result_processing.h"

namespace alg
{

	class AbsSizeFilter : public ResultProcessing
	{
		public:
		
		//Methods
		AbsSizeFilter();
		~AbsSizeFilter();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "abs_size_filter"; }
		inline std::string description() const { return "Object absolute size filter"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
