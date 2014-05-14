#include <ada.h>

using namespace cv;

Mat AdaBoost::nextFrame(Context& context)
{
    // Read current frame and convert it to IplImage
    //Mat& frame_mat = context.curr_frame;
    //IplImage frame_ipl = frame_mat;
    //IplImage* frame = &frame_ipl;
    //map<string,float>::iterator it;
    //vector<IplImage*> images;
    vector<Mat> images;
    Mat result = Mat::zeros(context.frame.rows, context.frame.cols, CV_8UC1);
    //IplImage* result = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,1);
    for ( unsigned int c=0;c<classifiers.size();c++ )
    {
        images.push_back(classifiers.at(c)->nextFrame(context));
    }
    
    for (int i=0;i<context.frame.rows;i++)
    {
        for (int j=0;j<context.frame.cols;j++)
        {
	  float sum=0;
	  float value;
	  for (unsigned int k=0;k<classifiers.size();k++ )
	  {
	      value = images[k].at<uchar>(i,j);//cvGetReal2D(images[k],i,j);
	      if(value>127)sum+= classifier_weights[k];
	      else sum-=classifier_weights[k];
	  }
	  result.at<uchar>(i,j) = (sum>0)?255:0;
	  //cvSetReal2D(result,i,j,(sum>0)?255:0);
        }
      
    }
    //for ( int c=0;c<classifiers.size();c++ )cvReleaseImage(&images[c]);
    
    //Mat tmp;
    //IplImage* result1 = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_LABEL,1);
    //cout << result1->depth << endl;
    //cout << result->nChannels << endl;
    //cvLabel(result,result1, blobs);
    // Convert result to Mat
    //Mat result_mat(result, true);
    // Release memory
    //cvReleaseImage(&result);
    //cvReleaseImage(&result1);
    // Set context and return
    context.motion_output = result;
    return result;

}
