// Class for cascading image preprocessing filters

#ifndef ALG_BLOBPROCESSING_H
#define ALG_BLOBPROCESSING_H

#include <cv.h>
#include <cvblob.h>
#include <highgui.h>
#include <iostream>
#include "../algorithm.h"
#include <log.h>
#include <opencv_utils.h>

namespace alg
{

	class BlobProcessing : public alg::Algorithm
	{
		private:

		// List of filters to apply
		std::vector<BlobProcessing*> sub_filters;

		public:
			
		virtual string name() const { return "null_blob_proc"; }
		virtual string type() const { return "blob_filtering"; }
		virtual string description() const { return "No blob filtering"; }
		virtual string version() const { return "1.0"; }
		virtual int executionTime() const { return 0; }
		virtual string descriptionExecutionTime() const { return "Average time to process a blob"; }
		virtual int ram() const { return 0; }
		virtual string input() const { return "cvb::CvBlobs"; }
		virtual string output() const { return "cvb::CvBlobs"; }

		virtual ~BlobProcessing() {}
		

		// Remove invalid blobs
		static void removeInvalidBlobs(cvb::CvBlobs& blobs, unsigned int max_width = INT_MAX, unsigned int max_height = INT_MAX);
		
		// Process next frame, by applying each sub-filter (actual filter implementations will put their code here)
		virtual void filterBlobs(const cv::Mat& frame, const cvb::CvBlobs& blobs, cvb::CvBlobs& filtered_blobs, const cv::Mat* next_frame = NULL)
		{
			// Initialize input to the filter chain
		       	cvCloneBlobs(blobs, filtered_blobs);
			// Go through all filters
			for(vector<BlobProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
			{
				// Compute output
				//Log::d() << "applying filter " << (*it)->name() << ", initial blobs: " << filtered_blobs.size();
				//cv::Mat init = rebuildBlobs(filtered_blobs, 320, 240);
				//cv::imshow("init", init);
				cvb::CvBlobs sub_output;
				(*it)->filterBlobs(frame, filtered_blobs, sub_output, next_frame);
				cvReleaseBlobs(filtered_blobs);
				filtered_blobs = sub_output;
				//Log::d() << ", final blobs: " << filtered_blobs.size() << endl;
				//cv::Mat final = rebuildBlobs(filtered_blobs, 320, 240);
				//cv::imshow("final", final);
				//cv::waitKey(0);
			}
		}

		// Add sub-filter
		inline void addSubFilter(BlobProcessing* sub_filter) { sub_filters.push_back(sub_filter); }

		// Get sub-filters
		inline std::vector<BlobProcessing*> getSubFilters() { return sub_filters; }
		
		// Get sub-filters as alg::Algorithm
		std::vector<alg::Algorithm*> getSubFiltersAsAlgs();
	};

}

#endif
