// Class for cascading image preprocessing filters

#ifndef ALG_PREPROCESSING_H
#define ALG_PREPROCESSING_H

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include "../algorithm.h"

//class PreProcessing;

namespace alg
{

	class PreProcessing : public alg::Algorithm
	{
		private:

		// List of filters to apply
		std::vector<PreProcessing*> sub_filters;

		public:
			
		virtual string name() const { return "null_preproc"; }
		virtual string type() const { return "frame_preproc"; }
		virtual string description() const { return "No preprocessing"; }
		virtual string version() const { return "1.0"; }
		virtual int executionTime() const { return 0; }
		virtual string descriptionExecutionTime() const { return "Average time to process a frame"; }
		virtual int ram() const { return 0; }
		virtual string input() const { return "cv::Mat"; }
		virtual string output() const { return "cv::Mat"; }

		virtual ~PreProcessing() {}
		
		// Process next frame, by applying each sub-filter (actual filter implementations will put their code here)
		virtual cv::Mat nextFrame(const cv::Mat& frame)
		{
			// Initialize input to the filter chain
			cv::Mat input_to_next = frame.clone();
			// Go through all filters
			for(vector<PreProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
			{
				// Compute output
				input_to_next = (*it)->nextFrame(input_to_next);
			}
			// Return result
			//imshow("itn", input_to_next); waitKey(0);
			return input_to_next;
		}

		// Add sub-filter
		inline void addSubFilter(PreProcessing* sub_filter) { sub_filters.push_back(sub_filter); }

		// Get sub-filters
		inline std::vector<PreProcessing*> getSubFilters() { return sub_filters; }
		
		// Get sub-filters as alg::Algorithm
		std::vector<alg::Algorithm*> getSubFiltersAsAlgs();
	};

}

#endif
