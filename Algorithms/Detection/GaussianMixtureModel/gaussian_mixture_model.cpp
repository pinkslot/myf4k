// Gaussian mixture model detection class

#include "gaussian_mixture_model.h"

namespace alg
{

	GaussianMixtureModel::GaussianMixtureModel()
	{
		// Setup parameters
		parameters.add<float>("running_avg_alpha", 0.02);
		parameters.add<float>("threshold_1", 220);
		parameters.add<float>("threshold_2", 30);
		parameters.add<float>("threshold", CV_BGFG_MOG_BACKGROUND_THRESHOLD);
		// Initialize variables
		//k = 1;
		greyImage = NULL; 
		smooth = NULL; 
		dilate = NULL; 
		erode = NULL; 
		and_ = NULL; 
		movingAverage = NULL;
		difference = NULL;
		temp = NULL;
		old_image = NULL;
		foreground = NULL;
		connected = NULL;
		//size = cvSize(640, 480);
		//capture = 0;
		//time0=0,time1=0;	
		bg_model = NULL;
		//stop = 0;
		//numFishFrame = 0;
		//big = 0;
		//big_index = 0;
		//big_size = 0;
		//max_big_size = 0;
		//mill=0, ratio=1, ratio_old=0,diff=0, fps=0; 		
		//time0 = ((double)clock())/CLOCKS_PER_SEC;
		initialized = false;
	}

	Mat GaussianMixtureModel::nextFrame(Context& context)
	{
		// Copy parameters to local variables
		float running_avg_alpha = parameters.get<float>("running_avg_alpha");
		float threshold_1 = parameters.get<float>("threshold_1");
		float threshold_2 = parameters.get<float>("threshold_2");
		float threshold = parameters.get<float>("threshold");
		//float dilate_cycles_1 = parameters.get<float>("dilate_cycles_1");
		//float erode_cycles_1 = parameters.get<float>("erode_cycles_1");
		//float dilate_cycles_2 = parameters.get<float>("dilate_cycles_2");
		// Create copy of frame
		Mat frame_mat = context.preproc_output.clone();
		IplImage frame_ipl = frame_mat;
		IplImage *frame = &frame_ipl;
		// Initialize variables and images
		if(! initialized)
		{
			// Allocate image buffers
			greyImage = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
			smooth = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
			dilate = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
			erode = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
			and_ = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
			movingAverage = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_32F, 3);
			foreground = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1);
			connected = cvCreateImage(cvSize(frame->width,frame->height), IPL_DEPTH_8U, 3);
			// Create background model
			CvGaussBGStatModelParams params;
			params.win_size      = CV_BGFG_MOG_WINDOW_SIZE;
			params.bg_threshold  = threshold;//CV_BGFG_MOG_BACKGROUND_THRESHOLD;
			params.std_threshold = CV_BGFG_MOG_STD_THRESHOLD;
			params.weight_init   = CV_BGFG_MOG_WEIGHT_INIT;
			params.variance_init = 15;//CV_BGFG_MOG_SIGMA_INIT*CV_BGFG_MOG_SIGMA_INIT;
			params.minArea       = CV_BGFG_MOG_MINAREA;
			params.n_gauss       = CV_BGFG_MOG_NGAUSSIANS;
			bg_model = cvCreateGaussianBGModel(frame, &params);
			cvConvertScale(frame, movingAverage, 1.0, 0.0);
			old_image = cvCreateImage( cvSize(frame->width,frame->height), IPL_DEPTH_8U, 1); 
			// Set flag
			initialized = true;
		}
		else
		{
			// Update background model
			cvRunningAvg(frame, movingAverage, running_avg_alpha, NULL);  // Updating moving averange background
			cvUpdateBGStatModel( frame, bg_model );           // Updating mixture gaussian model
		}
		// Copy original image
		difference = cvCloneImage(frame);
		temp = cvCloneImage(frame);
		//Convert the scale of the moving average.
		cvConvertScale(movingAverage,temp, 1.0, 0.0);
		//Minus the current frame from the moving average.
		cvAbsDiff(frame,temp,difference);
		//Convert the image to grayscale. Thresholding of the moving average algorithm to identify blobs
		cvCvtColor(difference,greyImage,CV_BGR2GRAY);
		//cvShowImage("greyImage", greyImage);
		cvThreshold(greyImage, greyImage, threshold_1, 255, CV_THRESH_BINARY_INV);
		//cvShowImage("thr greyImage", greyImage);
		//Convert the image to black and white. Thresholding of gaussian algorithm to identify blobs
		//cvShowImage("fg", bg_model->foreground);
		cvThreshold(bg_model->foreground, bg_model->foreground, threshold_2, 255, CV_THRESH_BINARY);
		//cvShowImage("thr fg", bg_model->foreground);
		cvSmooth(bg_model->foreground, foreground,CV_MEDIAN,3,0);
		// Thresholding of the grabbed frame to identify the dearker parts of the farme --> This one is done  to remove the light effects
		cvAnd(greyImage,foreground,greyImage,NULL);
		//waitKey(0);
		//cvNamedWindow("1: original"); cvShowImage("1: original", greyImage);
		//cvNamedWindow("old"); cvShowImage("old", old_image);
		//Dilate and erode to get fishes blobs

		/*cvSmooth(greyImage,smooth,CV_MEDIAN,17,0,0,0);
		//cvNamedWindow("2: smooth 1"); cvShowImage("2: smooth 1", smooth);
		
		cvDilate(smooth, dilate, 0, dilate_cycles_1);
		//cvNamedWindow("3: dilate 2"); cvShowImage("3: dilate 2", dilate);

		cvErode(dilate, erode, 0, erode_cycles_1);
		//cvNamedWindow("4: erode 3"); cvShowImage("4: erode 3", erode);

		cvDilate(erode, erode, 0, dilate_cycles_2);

		cvAnd(erode, greyImage, and_, NULL);
		//cvNamedWindow("5: 4 and old"); cvShowImage("5: 4 and old", and_);
	//	cvShowImage("aaa", and_); cvWaitKey(0);

		//cvWaitKey(0);


		cvCopy(erode, old_image, NULL);
		//if(big != 1);// && k > big_index)
		//doDetection(and_, connected, frame, blobs);*/
		
		// Clean local images
		cvReleaseImage(&difference);
		cvReleaseImage(&temp);
		//cvReleaseImage(&frame);
		// Convert to result
		Mat result(greyImage, true);
		context.motion_output = result;
		return result;
	}	

	GaussianMixtureModel::~GaussianMixtureModel()
	{
		cvReleaseImage(&connected);
		cvReleaseImage(&greyImage);
		cvReleaseImage(&smooth);
		cvReleaseImage(&dilate);
		cvReleaseImage(&erode);
		cvReleaseImage(&and_);
		cvReleaseImage(&difference);
		cvReleaseImage(&temp);
		cvReleaseImage(&old_image);
		cvReleaseImage(&movingAverage);
		cvReleaseImage(&foreground);
	}

	/*void GaussianMixtureModel::doDetection(IplImage* src, IplImage *final, IplImage *original, cvb::CvBlobs& blobs)
	{
		IplImage *out = cvCreateImage(cvSize(src->width, src->height), IPL_DEPTH_LABEL, 1);
		cvb::cvLabel(src, out, blobs);
		cvReleaseImage(&out);
	}*/

}
