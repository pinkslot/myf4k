// Class for cascading result post-processing filters
#ifndef ALG_RESULT_PROCESSING_H
#define ALG_RESULT_PROCESSING_H

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "../algorithm.h"
#include <context.h>
#include <results.h>

namespace alg
{

	class ResultProcessing : public Algorithm
	{
		private:

		// List of filters to apply
		std::vector<ResultProcessing*> sub_filters;

		public:
			
		virtual string name() const { return "null_resultproc"; }
		virtual string type() const { return "result_processing"; }
		virtual string description() const { return "No result processing"; }
		virtual string version() const { return "1.0"; }
		virtual int executionTime() const { return 0; }
		virtual string descriptionExecutionTime() const { return "Average time to process a result set"; }
		virtual int ram() const { return 0; }
		virtual string input() const { return "pr::Results"; }
		virtual string output() const { return "pr::Results"; }
		
		// Process next frame, by applying each sub-filter (actual filter implementations will put their code here)
		virtual pr::Results processResults(Context& context)
		{
			// Initialize output results to input
			context.filtered_results = context.results;
			// Store original results
			pr::Results original_results = context.results;
			// Go through all filters
			for(vector<ResultProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
			{
				// Compute output
				pr::Results input_to_next = (*it)->processResults(context);
				// Set output as new input
				context.results = input_to_next;
			}
			// Restore original results
			context.results = original_results;
			// Return result
			return context.filtered_results;
		}

		// Add sub-filter
		inline void addSubFilter(ResultProcessing* sub_filter) { sub_filters.push_back(sub_filter); }

		// Get sub-filters
		inline std::vector<ResultProcessing*> getSubFilters() { return sub_filters; }
		
		// Get sub-filters as alg::Algorithm
		std::vector<alg::Algorithm*> getSubFiltersAsAlgs();
	};

}

#endif
