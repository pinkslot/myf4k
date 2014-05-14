#include "MeanBGS.h"

using namespace cv;

namespace alg
{
	MeanBGS::MeanBGS(void)
	{
		// Setup parameters
		parameters.add<int>("threshold", 3*30*30);
		parameters.add<float>("high_low_ratio", 2);
		parameters.add<float>("alpha", 1e-6f);
		parameters.add<int>("learning_frames", 30);
		// Set init flag to true
		init = true;
	}

	Mat MeanBGS::nextFrame(Context& context)
	{
		// Get frame
		Mat frame = context.preproc_output;
		// Read the image into frame_data
		IplImage ipl_frame = frame;
		frame_data = &ipl_frame;
		// Check initialization
		if(init)
		{
			// Set parameters
			Algorithms::BackgroundSubtraction::MeanParams params;
			params.SetFrameSize(frame.cols, frame.rows);
			params.LowThreshold() = parameters.get<int>("threshold");
			params.HighThreshold() = parameters.get<float>("high_low_ratio")*params.LowThreshold();
			params.Alpha() = parameters.get<float>("alpha");
			params.LearningFrames() = parameters.get<int>("learning_frames");
			bgs.Initalize(params);
			// Initialize images
			low_threshold_mask = cvCreateImage(cvSize(frame.cols, frame.rows), IPL_DEPTH_8U, 1);
			high_threshold_mask = cvCreateImage(cvSize(frame.cols, frame.rows), IPL_DEPTH_8U, 1);
			frame_data.ReleaseMemory(false);
			// Initialize model
			bgs.InitModel(frame_data);
			// Clear flag
			init = false;
		}
		// Perform background subtraction
		bgs.Subtract(context.frame_num, frame_data, low_threshold_mask, high_threshold_mask);
		// Convert motion mask to Mat
		Mat bkg_sub(low_threshold_mask.Ptr(), true);
		// Remove isolated white pixels
		Mat median_out;
		medianBlur(bkg_sub, median_out, 3);
		// Show image
		//imshow("segm", median_out);
		//waitKey(0);
		// Set to context and return
		context.motion_output = median_out;
		return median_out;
	}
}
