#ifndef ALG_BORDER_FILTER_H
#define ALG_BORDER_FILTER_H

#include "result_processing.h"

namespace alg
{

	class BorderFilter : public ResultProcessing
	{
		public:
		
		//Methods
		BorderFilter();
		virtual ~BorderFilter();
		
		pr::Results processResults(Context& context);

		// Implement algorithm information
		inline std::string name() const { return "border_filter"; }
		inline std::string description() const { return "Filter for objects touching the frame border"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
	};

}

#endif
