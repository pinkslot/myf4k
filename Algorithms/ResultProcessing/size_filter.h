#ifndef ALG_SIZE_FILTER_H
#define ALG_SIZE_FILTER_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "result_processing.h"

namespace alg
{

	class SizeFilter : public ResultProcessing
	{
		public:
		
		//Methods
		SizeFilter();
		~SizeFilter();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "size_filter"; }
		inline std::string description() const { return "Object size filter"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
