#include "CodeBook.h"

using namespace cv;

namespace alg{
CodeBook::CodeBook(void) : _init(true)
{
	nframes=0;
	ch[0]=true;
	ch[1]=true;
	ch[2]=true;
	
	rawImage = 0; 
	yuvImage = 0;
	ImaskCodeBook = 0;
	ImaskCodeBookCC = 0;
	parameters.add<int>("mod_min", 3);
	parameters.add<int>("mod_max", 10);
	parameters.add<int>("cb_bound", 10);
	parameters.add<float>("train_frames",100);

}


CodeBook::~CodeBook(void)
{
}


Mat CodeBook::nextFrame(Context& context)
{
	if(_init)
	{
		_init = false;
		model = cvCreateBGCodeBookModel();
		model->modMin[0] = model->modMin[1] = model->modMin[2] = parameters.get<int>("mod_min");
		model->modMax[0] = model->modMax[1] = model->modMax[2] = parameters.get<int>("mod_max");
		model->cbBounds[0] = model->cbBounds[1] = model->cbBounds[2] = parameters.get<int>("cb_bound");
		nframesToLearnBG = parameters.get<int>("train_frames");
	}

	++nframes;

	IplImage frame_ipl = context.preproc_output;
	const IplImage* frame = &frame_ipl;
    
        yuvImage = cvCloneImage(frame);   
	ImaskCodeBook = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
    	ImaskCodeBookCC = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 );
	if( nframes == 1 && frame )
	{
	    cvSet(ImaskCodeBook,cvScalar(255));
	}
	if( frame )
    	{
            cvCvtColor( frame, yuvImage, CV_BGR2YCrCb );
            if(nframes-1 < nframesToLearnBG  )
	    {
	        cvBGCodeBookUpdate( model, yuvImage );
		cvReleaseImage(&yuvImage);
		cvReleaseImage(&ImaskCodeBook);
	        cvReleaseImage(&ImaskCodeBookCC);
		Mat result = Mat::zeros(frame->height, frame->width, CV_8UC1);
		context.motion_output = result;
		return result;
	    }
            else if( nframes-1 == nframesToLearnBG  )
	    {
                cvBGCodeBookClearStale( model, model->t/2 );
		cvReleaseImage(&yuvImage);
		cvReleaseImage(&ImaskCodeBook);
		cvReleaseImage(&ImaskCodeBookCC);
		Mat result = Mat::zeros(frame->height, frame->width, CV_8UC1);
		context.motion_output = result;
		return result;
            }
            else if( nframes-1 >= nframesToLearnBG  )
            {
		//IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);
		IplConvKernel* morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL);
                cvBGCodeBookDiff( model, yuvImage, ImaskCodeBook );
                cvCopy(ImaskCodeBook,ImaskCodeBookCC);	
		tmp = cvCreateImage( cvGetSize(frame), IPL_DEPTH_8U, 1 ); 
                cvSegmentFGMask( ImaskCodeBookCC );
		//cvMorphologyEx(ImaskCodeBookCC, tmp, NULL, morphKernel, CV_MOP_OPEN, 1);
		//cvLabel(tmp, labelImg, blobs);
		// Convert to Mat
		//Mat result(tmp, true);
		Mat result(ImaskCodeBookCC, true);
		cvReleaseImage(&tmp);
		//cvReleaseImage(&labelImg);
		cvReleaseImage(&yuvImage);
		cvReleaseImage(&ImaskCodeBook);
		cvReleaseImage(&ImaskCodeBookCC);
		// Set context and return
		context.motion_output = result;
		return result;
            }

        }
		Mat result = Mat::zeros(frame->height, frame->width, CV_8UC1);
		context.motion_output = result;
		return result;
		
		
}
}
