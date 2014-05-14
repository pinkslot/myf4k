#ifndef ALG_LIFETIME_FILTER_H
#define ALG_LIFETIME_H

#include <vector>
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include "../algorithm.h"
#include "result_processing.h"

namespace alg
{

	class LifetimeFilter : public ResultProcessing
	{
		public:
		
		//Methods
		LifetimeFilter();
		~LifetimeFilter();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "lifetime_filter"; }
		inline std::string description() const { return "Object lifetime filter"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
