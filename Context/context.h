// Manages processing context for current processing cycle
#ifndef CONTEXT_H
#define CONTEXT_H

#include <cv.h>
#include <cvblob.h>
#include <results.h>
#include <string.h>

class Context
{
	public:
	
	// Current frame number
	int frame_num;

	// Previous, current and next frame
	cv::Mat prev_frame, frame, next_frame;

	// Frame without time
	cv::Mat frame_no_time;

	// Pre-processing output
	cv::Mat preproc_output;

	// Motion detection output
	cv::Mat motion_output;

	// Post-processing output
	cv::Mat postproc_output;

	// Original blobs
	cvb::CvBlobs original_blobs;

	// Blob-filtering output
	cvb::CvBlobs filtered_blobs;

	// Results
	pr::Results results;

	// Filtered results
	pr::Results filtered_results;

	std::string file_name;

};

#endif
