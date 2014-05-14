// Class for cascading image post-processing filters
#ifndef ALG_POSTPROCESSING_H
#define ALG_POSTPROCESSING_H

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "../algorithm.h"
#include <context.h>

namespace alg
{

	class PostProcessing : public alg::Algorithm
	{
		private:

		// List of filters to apply
		std::vector<PostProcessing*> sub_filters;

		public:
			
		virtual string name() const { return "null_postproc"; }
		virtual string type() const { return "post_processing"; }
		virtual string description() const { return "No post-processing"; }
		virtual string version() const { return "1.0"; }
		virtual int executionTime() const { return 0; }
		virtual string descriptionExecutionTime() const { return "Average time to process a frame"; }
		virtual int ram() const { return 0; }
		virtual string input() const { return "cv::Mat"; }
		virtual string output() const { return "cv::Mat"; }
		
		// Process next frame, by applying each sub-filter (actual filter implementations will put their code here)
		virtual cv::Mat nextFrame(Context& context)
		{
			// In case no subfilters exist, initialize output as input
			context.postproc_output = context.motion_output.clone();
			// Store original motion_output
			cv::Mat original_motion_output = context.motion_output;
			// Go through all filters
			for(vector<PostProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
			{
				// Compute output
				cv::Mat input_to_next = (*it)->nextFrame(context);
				// Set output as new input
				context.motion_output = input_to_next;
			}
			// Restore motion_output
			context.motion_output = original_motion_output;
			// Return result
			return context.postproc_output;
		}

		// Add sub-filter
		inline void addSubFilter(PostProcessing* sub_filter) { sub_filters.push_back(sub_filter); }

		// Get sub-filters
		inline std::vector<PostProcessing*> getSubFilters() { return sub_filters; }
		
		// Get sub-filters as alg::Algorithm
		std::vector<alg::Algorithm*> getSubFiltersAsAlgs();
	};

}

#endif
