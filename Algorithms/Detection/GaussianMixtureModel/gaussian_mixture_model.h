// Gaussian mixture model detection class, implementing the generic detection class interface

#ifndef GAUSSIAN_MIXTURE_MODEL_H
#define GAUSSIAN_MIXTURE_MODEL_H

#include <vector>
#include "../detection.h"
#include <cvblob.h>
#include <cv.h>
#include <highgui.h>
#include <cvaux.h>

using namespace std;
using namespace cv;

namespace alg
{

	class GaussianMixtureModel : public Detection
	{
		public:
		
		inline string name() const { return "agmm"; }
		inline string description() const { return "Adaptive Gaussian Mixture Model"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		// Declare variables used by the algorithm
		//int k;
		IplImage* greyImage; 
		IplImage *smooth, *dilate, *erode, *and_;
		IplImage* movingAverage;
		IplImage* difference;
		IplImage* temp;
		IplImage *old_image;
		IplImage* foreground;
		IplImage* connected;
		CvSize size;
		//CvCapture* capture;
		//double time0,time1;	
		CvSize frame_size;
		CvBGStatModel* bg_model;
		int stop;
		//int numFishFrame;
		//int big;
		//int big_index;
		//float big_size;
		//float max_big_size;
		char wow[65];
		//double mill, fr, ratio, ratio_old, diff, fps;

		// Flag for checking algorithm initialization
		bool initialized;

		// Constructor, for variable initialization
		GaussianMixtureModel();

		// Destructor, for memory deallocation
		~GaussianMixtureModel();

		// Detection method
		void doDetection(IplImage* src, IplImage *final, IplImage *original, cvb::CvBlobs& blobs);

		// Analyze next frame and return objects:
		// - frame: next input frame, on which to run the detection algorithm
		// - blobs: list of blobs;
		cv::Mat nextFrame(Context& context);
	};

}

#endif
