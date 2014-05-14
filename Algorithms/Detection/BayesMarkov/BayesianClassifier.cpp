#include "BayesianClassifier.h"


BayesianClassifier::BayesianClassifier(void)
{
    gamma = 0;
    hd=25;
    hr=16;
    first_frame = true;

    one_alpha = 1-ALPHA;
    frames_in_background =0;
    frames_in_foreground =0;
    h_coefficient = 6.3158/1000000;
}


BayesianClassifier::~BayesianClassifier(void)
{

}

void BayesianClassifier::nextFrame(const IplImage* frame,cvb::CvBlobs& blobs)
{

    IplImage* float_image_fore = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_32F,1);
    IplImage* float_image_back = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_32F,1);
    if (gamma==0)
    {
        gamma = 1./255./255./255./frame->width/frame->height;
        cout << "gamma: " << gamma << endl;
        thr = log(gamma);
        //n = 1/(frame->width*frame->height);
        alpha_gamma = ALPHA*gamma;
        cout << "alpha_gamma" << ALPHA*gamma << endl;
        for (int i=0;i<frame->height;i++)
        {
            vector<double> tmp;
            for (int j=0;j<frame->width;j++)
            {
                tmp.push_back(0);
            }
            likelihood_ratio_map.push_back(tmp);
        }
    }

    cvShowImage("Input",frame);
    cvWaitKey(1);
    double sum = 0;
    IplImage* result = cvCreateImage(cvSize(frame->width,frame->height),IPL_DEPTH_8U,1);
    deque<BayesFeature> tmp_foreground;
    for (int i=0;i<frame->height;i++)
    {
        for (int j=0;j<frame->width;j++)
        {
            cvSetReal2D(result,i,j,0);
        }
    }
    for (int i=0;i<frame->height;i++)
    {
        for (int j=0;j<frame->width;j++)
        {
            BayesFeature bp = new BayesPoint();
            bp->x=j;
            bp->y=i;
            CvScalar cvs = cvGet2D(frame,i,j);
            bp->b = cvs.val[0];
            bp->g = cvs.val[1];
            bp->r = cvs.val[2];
	    bp->is_in_background = true;
	    bp->is_in_foreground = false;
            if (first_frame)
            {
                background.push_back(bp);
                continue;
            }
            double pb = 0;
            double pf = 0;
            int pixel = j+ frame->width*i;
            for (int l=0;l<background.size();l++)
            {
                double result_pb = get_bkg_exp(bp,background[l]);
                if (result_pb==std::numeric_limits<double>::infinity())continue;
                pb+= result_pb;

            }
            
	    pb/=background.size();
	    float lr = 0;
            if ( foreground.size()>0)
            {
                for (int l=0;l<foreground.size();l++)
                {
                    if (foreground[l]==NULL)continue;
		    
                    pf += get_fore_exp(bp,foreground[l]);
		   
                }
                pf/=foreground.size();
	
		lr = this->get_likelihood_ratio(pb,pf);
                likelihood_ratio_map[i][j] = lr;
                
            }
            else if (pf==0.0)
            {
	        
	
                if (pb<0.1)
                {
		  
		  lr = 1;
		}
		else lr=-1;
            }
          
            //cout << pb << endl;
	    //cout << pf <<endl;
            //cout << pf << endl;
	    cvSetReal2D(float_image_fore,i,j,pf);
	    cvSetReal2D(float_image_back,i,j,pb);
            
            cout << "Likelihood ratio: " << lr << endl;
	    int res = get_delta(lr);
	    //cout << lr << endl;
	    cout << "PF: " << pf << endl;
	    cout << "PB: " << pb << endl;
	    cvWaitKey(0);
	    if (lr>=0)
	    {
	      tmp_foreground.push_back(bp);
	      bp->is_in_foreground = true;
	    }
	    cvSetReal2D(result,i,j,res);

            
            //cout << "PF: " << pf << endl;
            background.push_back(bp);

            //if(exp_b!=0)std::cout << exp_b << endl;

        }

    }
    cout << "Pixels in foreground " <<  foreground.size() << " Adding " << tmp_foreground.size() << endl;
    cout << "Pixels in background " <<  background.size() << endl;
    int newly_added_fore = tmp_foreground.size();
    for (int o=0;o<newly_added_fore;o++)
    {
        BayesFeature bf = tmp_foreground[0];
        tmp_foreground.pop_front();
        foreground.push_back(bf);
    }

    IplImage* image_sc = cvCreateImage( cvSize(320 , 240 ), frame->depth, 1 );
    cvResize(result,image_sc);
    cvShowImage("Output",image_sc);

    cvReleaseImage(&result);
    cvReleaseImage(&image_sc);

    if (frames_in_background<RHO_B)frames_in_background++;
    else {
        int removed = 0;
	
        for (int i=0;i<frame->width* frame->height;i++)
        {
	 
            BayesFeature bf = background[0];
	    if(!bf->is_in_foreground)
	    {
	      delete bf;
	    }
	    else bf->is_in_background = false;
	    
            background.pop_front();
            removed++;
        }

        cout << "Deleting pixels from background "<<  RHO_B << " frames ago. Deleted "<< removed << endl;
    }

    if (frames_in_foreground<RHO_F)frames_in_foreground++;
    else {
        int removed = 0;
        if (foreground.size()>0)
        {
            BayesFeature bf = foreground[0];
	    if(!bf->is_in_background)
	    {
	      delete bf;
	    }
	   
            while (foreground.size()>0) {
	      bf = foreground[0];
	      if(bf==NULL)break;
	      if(!bf->is_in_background)
	      {
		delete bf;
	      }
	      else bf->is_in_foreground = false;
	      foreground.pop_front();
	      bf = foreground[0];
	      removed++;
            } 
            foreground.pop_front();


        }
        cout << "Deleting pixels from foreground "<<  RHO_F << " frames ago. Deleted "<< removed << endl;
    }
    if(!first_frame)    foreground.push_back(NULL);
    first_frame = false;
    
    cvNormalize(float_image_back,float_image_back,0,1,CV_MINMAX);
    cvNormalize(float_image_fore,float_image_fore,0,1,CV_MINMAX);

    IplImage *destination2 = cvCreateImage( cvSize(320,240),IPL_DEPTH_32F, 1 );

    cvResize(float_image_back, destination2);
    cvShowImage("Float Image Back",destination2);
    IplImage *destination3= cvCreateImage( cvSize(320,240),IPL_DEPTH_32F, 1 );
    cvShowImage("Bayes result",result);
    cvResize(float_image_fore, destination3);
    cvShowImage("Float Image Fore",destination3);
    cvReleaseImage(&float_image_back);
    cvReleaseImage(&float_image_fore);
    cvReleaseImage(&destination2);
    cvWaitKey(1);

}

IplImage* BayesianClassifier::nextFrame(IplImage* inimage)
{
    return inimage;
}
