#ifndef ALG_CERTAINTY_FILTER_H
#define ALG_CERTAINTY_FILTER_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "result_processing.h"

namespace alg
{

	class CertaintyFilter : public ResultProcessing
	{
		public:
		
		//Methods
		CertaintyFilter();
		~CertaintyFilter();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "certainty_filter"; }
		inline std::string description() const { return "Certainty filter"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
