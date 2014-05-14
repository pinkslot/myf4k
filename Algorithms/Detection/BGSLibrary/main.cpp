/****************************************************************************
*
* main.cpp
*
* Purpose: Simple example illustrating use of BGS library.
*
* Author: Donovan Parks, June 2008
*
* Note: You will need to install the HUFFY codex at: 
*					http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html
*
******************************************************************************/




#include <iostream>

#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "AdaptiveMedianBGS_impl.h"
#include "GrimsonGMM_impl.h"
#include "ZivkovicAGMM_impl.h"
#include "MeanBGS_impl.h"
#include "WrenGA_impl.h"
#include "PratiMediodBGS_impl.h"
#include "Eigenbackground_impl.h"

using namespace cv;

enum RESULT_TYPE { IMAGE_DMS, IMAGE_SM_WALLFLOWER, VIDEO };

int main(int argc, const char* argv[])
{
	// read data from AVI file
	CvCapture* readerAvi = cvCaptureFromFile(argv[1]);
	if(readerAvi == NULL)
	{
		std::cerr << "Could not open AVI file." << std::endl;
		return 0;
	}

	// retrieve information about AVI file
	cvQueryFrame(readerAvi); 
	int width	= (int) cvGetCaptureProperty(readerAvi, CV_CAP_PROP_FRAME_WIDTH);
	int height = (int) cvGetCaptureProperty(readerAvi, CV_CAP_PROP_FRAME_HEIGHT);
	int fps = (int) cvGetCaptureProperty(readerAvi, CV_CAP_PROP_FPS);
	int num_frames = (unsigned int) cvGetCaptureProperty(readerAvi,  CV_CAP_PROP_FRAME_COUNT);

	// setup marks to hold results of low and high thresholding
	BwImage low_threshold_mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	low_threshold_mask.Ptr()->origin = IPL_ORIGIN_BL;

	BwImage high_threshold_mask = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	high_threshold_mask.Ptr()->origin = IPL_ORIGIN_BL;

	// setup buffer to hold individual frames from video stream
	RgbImage frame_data;
	frame_data.ReleaseMemory(false);	// AVI frame data is released by with the AVI capture device

	// setup AVI writers (note: you will need to install the HUFFY codex at: 
	//   http://neuron2.net/www.math.berkeley.edu/benrg/huffyuv.html)
	CvVideoWriter* writerAvi = cvCreateVideoWriter("results.avi", CV_FOURCC('H', 'F', 'Y', 'U'),	
															fps, cvSize(width, height), 1);

	// setup background subtraction algorithm
	Algorithms::BackgroundSubtraction::AdaptiveMedianParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 40;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.SamplingRate() = 7;
	params.LearningFrames() = 30;

	Algorithms::BackgroundSubtraction::AdaptiveMedianBGS bgs;
	bgs.Initalize(params);

	/*
	Algorithms::BackgroundSubtraction::GrimsonParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 3.0f*3.0f;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 0.001f;
	params.MaxModes() = 3;

	Algorithms::BackgroundSubtraction::GrimsonGMM bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::ZivkovicParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 5.0f*5.0f;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 0.001f;
	params.MaxModes() = 3;

	Algorithms::BackgroundSubtraction::ZivkovicAGMM bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::MeanParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 3*30*30;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 1e-6f;
	params.LearningFrames() = 30;

	Algorithms::BackgroundSubtraction::MeanBGS bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::WrenParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 3.5f*3.5f;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.Alpha() = 0.005f;
	params.LearningFrames() = 30;

	Algorithms::BackgroundSubtraction::WrenGA bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::PratiParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 30;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.SamplingRate() = 5;
	params.HistorySize() = 16;
	params.Weight() = 5;

	Algorithms::BackgroundSubtraction::PratiMediodBGS bgs;
	bgs.Initalize(params);
	*/

	/*
	Algorithms::BackgroundSubtraction::EigenbackgroundParams params;
	params.SetFrameSize(width, height);
	params.LowThreshold() = 15*15;
	params.HighThreshold() = 2*params.LowThreshold();	// Note: high threshold is used by post-processing 
	params.HistorySize() = 100;
	params.EmbeddedDim() = 20;

	Algorithms::BackgroundSubtraction::Eigenbackground bgs;
	bgs.Initalize(params);
	*/

	// perform background subtraction of each frame 
	for(int i = 0; i < num_frames-1; ++i)
	{
		if(i % 100 == 0)
			std::cout << "Processing frame " << i << " of " << num_frames << "..." << std::endl;

		// grad next frame from input video stream
		if(!cvGrabFrame(readerAvi))
		{           
			std::cerr << "Could not grab AVI frame." << std::endl;
			return 0;
		}		
		frame_data = cvRetrieveFrame(readerAvi); 
		cvShowImage("aa", frame_data.Ptr());
		
		// initialize background model to first frame of video stream
		if (i == 0)
			bgs.InitModel(frame_data); 

		// perform background subtraction
		bgs.Subtract(i, frame_data, low_threshold_mask, high_threshold_mask);

		cvShowImage("bb", low_threshold_mask.Ptr());
		waitKey(0);
		// save results
		cvWriteFrame(writerAvi, low_threshold_mask.Ptr());

		// update background subtraction
		low_threshold_mask.Clear();	// disable conditional updating
		bgs.Update(i, frame_data, low_threshold_mask);
	}

	cvReleaseCapture(&readerAvi);
	cvReleaseVideoWriter(&writerAvi);
}
