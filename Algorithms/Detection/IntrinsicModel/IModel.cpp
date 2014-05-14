#include "IModel.h"
#define _LINUX_ 1

using namespace cvb;

namespace alg
{

	IModel::~IModel(void)
	{
		
	}

	cv::Mat IModel::nextFrame(Context& context)
	{
		// Copy parameters to local variables
		int sample_period = parameters.get<int>("sample_period");
		int background_window = parameters.get<int>("background_window");
		float threshold = parameters.get<float>("threshold");
		// Convert to frame
		IplImage frame_ipl = context.preproc_output;
		IplImage* frame = &frame_ipl;
			
			if(imageCount<=background_window)
			{

				width = frame->width;
				height = frame->height;
				area = width*height;
				tmp = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
				if(frame->nChannels==3)cvCvtColor(frame,tmp,CV_RGB2GRAY);
				else cvCopy(frame,tmp);
				imageCount++;
				this->initVector.push_back(tmp);
				if(imageCount==background_window)init(this->initVector);
				
				return cv::Mat::zeros(height, width, CV_8UC1);
			}
		
			IplImage* buff = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
			IplConvKernel* morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL);
			IplImage *processed_frame = cvCreateImage(cvGetSize(frame),IPL_DEPTH_8U,1);
			IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);


			if(frame->nChannels==3)cvCvtColor(frame,buff,CV_RGB2GRAY);
			else cvCopy(frame,buff);
			IplImage *c =NULL;
			IplImage *out ;
			IplImage *outBgrnd;
			IplImage* frametmp = FloatImage(frame);

			if(buff->nChannels>1)
				c = GetInGrayScale(buff);		
			
			else
				c = buff;
#ifdef _DEBUG_
			cvShowImage("Gray scale input Image",c);
#endif

			out = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
			IplImage* out1 = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
			if(l->framecount==sample_period-1)
			{
				l->add(c);
				
				l->UpdateForeground();
				cvCopy(l->GetForeground(),out);
				out = ExpImage(out);			
				l->framecount=0;
#ifndef _LINUX_
				l->hasNewItems = true;
#else
				l->UpdateBackGround();
#endif
			
			}
			else 
			{
				l->UpdateForeground(c);
				l->framecount++;
				cvCopy(l->GetForeground(),out);			
				ExpImage(out);
			}

			
			cvThreshold(out,out,threshold,1,CV_THRESH_BINARY_INV);
			cvConvertScale(out, l->tmp8u,255,0);	
			cvSmooth(l->tmp8u,l->tmp8u,CV_MEDIAN,3,3);
			//cvSmooth(l->tmp8u,processed_frame,CV_MEDIAN,7,7);

			/*cvMorphologyEx(l->tmp8u, processed_frame, NULL, morphKernel, CV_MOP_OPEN, 1);
			cvErode(processed_frame, processed_frame, NULL, 3);
			cvDilate(processed_frame, processed_frame, NULL, 3);
			cvErode(processed_frame, processed_frame, NULL, 2);
			cvDilate(processed_frame, processed_frame, NULL, 10);
			cvAnd(l->tmp8u, processed_frame, processed_frame, NULL);*/
			//cvShowImage("Output before CvBlobs", processed_frame);
		
			//int result = cvLabel(processed_frame, labelImg, blobs);
			cv::Mat result_mat(l->tmp8u, true);

			/*cvSmooth(out,out,CV_MEDIAN,3,3);
			//cvErode(out,out,0,2);
			cvConvertScaleAbs(out,l->tmp8u );
			cvFindContours(l->tmp8u, storage, &seq, sizeof(CvContour), CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cvPoint(0,0));

			CvPoint p;
			int idx=0;
			for(; seq; seq = seq->h_next) {
			CvRect boundbox = cvBoundingRect(seq);

				if((AREA_FACTOR*area)> boundbox.width*boundbox.height)continue;

				cvRectangle(out,cvPoint(boundbox.x,boundbox.y),cvPoint(boundbox.x+boundbox.width,boundbox.y+boundbox.height),cvScalar(127,0,0));
				cvRectangle(frametmp,cvPoint(boundbox.x,boundbox.y),cvPoint(boundbox.x+boundbox.width,boundbox.y+boundbox.height),cvScalar(0,0,0));
				cvSetImageROI(out,boundbox);
				
				IplImage* mask32 = cvCreateImage(cvSize(boundbox.width,boundbox.height),IPL_DEPTH_32F,1);
				IplImage* mask = cvCreateImage(cvSize(boundbox.width,boundbox.height),IPL_DEPTH_8U,1);
				cvCopy(out,mask32,NULL);
				cvConvertScaleAbs(mask32,mask);
				cvReleaseImage(&mask32);
				cvResetImageROI(out);
				bounding_boxes.push_back(boundbox);
				masks.push_back(mask);
			}

			
			
			cvReleaseMemStorage(&storage);*/
			outBgrnd = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
			cvCopy(l->GetBackGround(),outBgrnd);
			outBgrnd = ExpImage(outBgrnd);

#ifdef _DEBUG_
			cvShowImage("Background" , outBgrnd);
			cvShowImage("Foreground",out);
			cvShowImage("Input image with rectangles",frametmp);
#endif

			cvReleaseStructuringElement(&morphKernel);
			cvReleaseImage(&processed_frame);
			cvReleaseImage(&labelImg);
			cvReleaseImage(&out);
			cvReleaseImage(&out1);
			cvReleaseImage(&outBgrnd);
			cvReleaseImage(&frametmp);
			//cvReleaseImage(&buff);
			//cvWaitKey(1);
			// Set context and return
			context.motion_output = result_mat;
			return result_mat;
	}

	IplImage* IModel::GetInGrayScale(IplImage* img)
	{
		
		IplImage* gray = cvCreateImage(cvSize(width,height),IPL_DEPTH_8U,1);
		cvCvtColor(img,gray,CV_RGB2GRAY);
#ifdef _DEBUG_
		cvShowImage("Gray Scale Image",gray);
#endif
		cvReleaseImage(&img);
		return gray;
	}




	IplImage* IModel::ExpImage(IplImage* img)
	{
		
		cvExp(img,img);
		return img;
	}

	int IModel::initImageCount()
	{
		return 15;
	}


	void IModel::init(vector<IplImage*> imgArray)
	{
		width = ((IplImage*)imgArray.at(0))->width;
		height = ((IplImage*)imgArray.at(0))->height;
		l = new _BackGroundModeler(imgArray, parameters.get<int>("background_window"));
		

	}

	void IModel::init(IplImage* img)
	{
		width = img->width;
		height = img->height;

	}

	IplImage* IModel::FloatImage(const IplImage* img)
	{
		IplImage *fimage = cvCreateImage(cvGetSize(img),IPL_DEPTH_32F,1);
		for(int h=0;h<img->height;h++)
			for(int w=0;w<img->width;w++)
				((float *)(fimage->imageData + h*fimage->widthStep))[w*fimage->nChannels] = ((float)((uchar *)(img->imageData + h*img->widthStep))[w*img->nChannels]) / 255.0;
			
		
		return fimage;
	}

}
