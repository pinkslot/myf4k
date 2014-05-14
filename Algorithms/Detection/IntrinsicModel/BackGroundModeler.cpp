
#include "BackGroundModeler.h"
#define _LINUX_ 

#ifndef _LINUX_
#include <process.h>
#endif

//#include <opencv2/imgproc/imgproc_c.h>
#define LOG_ON_ADD 

namespace alg
{

	int comparefloat (const void * a, const void * b){
	  return ( *(float*)a - *(float*)b );
	}



	double f0[] = {1, 0, -1,
				   1, 0, -1,
				   1, 0, -1,};

	double f1[] = {1, 1, 1,
				   0, 0, 0,
				  -1, -1, -1};


#ifndef _LINUX_
	void BackGroundUpdateThread( void* lpParam )  
	{ 
		while(true){
			BackGroundModeler l = (BackGroundModeler)lpParam;
			l->hasNewItems = false;
			l->UpdateBackGround();
			while(!l->hasNewItems);
		}
	    
	} 

#endif



	_BackGroundModeler::_BackGroundModeler(vector<IplImage*> l, int bg_win)
	{
		// Set parameters
		background_window = bg_win;
		//area_factor = af;
		//sample_period = sp;
		// Initialize model	
		CvSize imgSize;
		width = ((IplImage*)l.at(0))->width;
		height = ((IplImage*)l.at(0))->height;
		imgSize.width = width;
		imgSize.height = height;
		this->current = NULL;
		this->First = NULL;
		this->Last = NULL;

		this->CurrentBackGround = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
		this->CurrentForeGround = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

		this->tmpfloat = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
		this->tmpbfloat= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

		this->tmpexp = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
		this->tmpbexp= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

		this->tmplog = cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
		this->tmpblog= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

		this->tmpbfilt1= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
		this->tmpbfilt2= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

		this->tmpfilt1= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);
		this->tmpfilt2= cvCreateImage(imgSize, IPL_DEPTH_32F, 1);

		this->tmp8u = cvCreateImage(imgSize, IPL_DEPTH_8U, 1);
		
		this->height = imgSize.height;
		this->width = imgSize.width;

		this->hasNewItems = false;
		this->BackGroundReady = false;
		this->updating=false;

		for(unsigned int i=0;i<l.size();i++)
		{
			this->add(l.at(i));
		}
		this->UpdateBackGround();
		this->reading = false;
		this->framecount=0;
		#ifndef _LINUX_
		_beginthread(BackGroundUpdateThread,0,(void*)this);
		#endif
	}




	void _BackGroundModeler::add(IplImage* img)
	{
		
		if(this->current==NULL)
		{
			this->current = (IplImageNode)malloc(sizeof(_LinkedListNode));
			this->Last = this->current;
			this->First = this->current;		
			this->current->img = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
			cvCopy(BFilter(BLogImage(BFloatImage(img))),this->current->img);	
			this->size=1;
			
			return;
		}
		
		this->Last->next = (IplImageNode)malloc(sizeof(_LinkedListNode));
		this->Last->next->previous = this->Last;
		this->Last = this->Last->next;

		this->Last->img = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
		cvCopy(BFilter(BLogImage(BFloatImage(img))),this->Last->img);	
		
			
		if(size==background_window)
		{
			cvReleaseImage(&this->First->img);
			this->First = this->First->next;
			if(this->current==this->First->previous)this->current=this->First;		
			free(this->First->previous);		
			return;
		}
		
		size++;
		
		return;
	}

	IplImage* _BackGroundModeler::getNext()
	{
		this->current = this->current->next;
		return this->current->img;
	}

	bool _BackGroundModeler::hasNext()
	{
		return (this->current!=this->Last);
	}

	bool _BackGroundModeler::hasPrevious()
	{
		return (this->current!=this->First);
	}

	IplImage* _BackGroundModeler::getPrevious()
	{
		this->current = this->current->previous;
		return this->current->img;
	}

	IplImage* _BackGroundModeler::getCurrent()
	{
		return this->current->img;
	}

	void _BackGroundModeler::setCurrentToLast()
	{
		this->current = this->Last;
	}

	void _BackGroundModeler::setCurrentToFirst()
	{
		this->current = this->First;
	}

	int _BackGroundModeler::initImageCount()
	{
		return 15;
	}


	CvSize _LinkedListNode::getSize()
	{
		return cvSize(this->img->width,this->img->height);
	}


	IplImage* _BackGroundModeler::UpdateBackGround()
	{
		int height = this->First->img->height;
		float* arr = new float[background_window];
		int width = this->First->img->width;
		IplImage* res1 = cvCreateImage(cvSize(width,height),IPL_DEPTH_32F,1);
		for(int h=0;h<height;h++)
			for(int w=0;w<width;w++)
			{			
				this->setCurrentToFirst();
				IplImage* img = this->First->img;
				arr[0] = (float) cvGetReal2D(img,h,w);
				for(int i=1;i<background_window;i++)
				{
					img = this->getNext();
					arr[i] = (float)cvGetReal2D(img,h,w);				
				}
				qsort(arr,15,sizeof(float),comparefloat);
				
				cvSetReal2D(res1,h,w,arr[background_window/2]);
			}
		
		
			cvCopy(res1,this->CurrentBackGround);

			cvReleaseImage(&res1);
			delete [] arr;
			
		return this->CurrentBackGround;
	}

	IplImage* _BackGroundModeler::GetBackGround()
	{
		return this->CurrentBackGround;
	}



	IplImage* _BackGroundModeler::FloatImage(IplImage* img)
	{
		
		for(int h=0;h<height;h++)
			for(int w=0;w<width;w++)
				((float *)(tmpfloat->imageData + h*tmpfloat->widthStep))[w*tmpfloat->nChannels] = ((float)((uchar *)(img->imageData + h*img->widthStep))[w*img->nChannels]) / 255.0;
			
		cvReleaseImage(&img);
		return tmpfloat;
	}

	IplImage* _BackGroundModeler::BFloatImage(IplImage* img)
	{
		
		for(int h=0;h<height;h++)
			for(int w=0;w<width;w++)
				((float *)(tmpbfloat->imageData + h*tmpbfloat->widthStep))[w*tmpbfloat->nChannels] = ((float)((uchar *)(img->imageData + h*img->widthStep))[w*img->nChannels]) / 255.0;
			
		cvReleaseImage(&img);
		return tmpbfloat;
	}



	IplImage* _BackGroundModeler::ExpImage(IplImage* img)
	{
		
		cvExp(img,tmpexp);

		return tmpexp;
	}

	IplImage* _BackGroundModeler::BExpImage(IplImage* img)
	{
		
		cvExp(img,tmpbexp);

		return tmpbexp;
	}

	IplImage* _BackGroundModeler::LogImage(IplImage* img)
	{
		
		cvLog(img,tmplog);	
		return tmplog;
	}

	IplImage* _BackGroundModeler::BLogImage(IplImage* img)
	{
		
		cvLog(img,tmpblog);
		
		return tmpblog;
	}



	void  _BackGroundModeler::UpdateForeground(IplImage* img)
	{	
		IplImage* fg = Filter(LogImage(FloatImage(img)));		
		cvSub(fg,this->GetBackGround(),fg);
		cvCopy(fg,this->CurrentForeGround);

	}

	void  _BackGroundModeler::UpdateForeground()
	{	
		cvSub(this->Last->img,this->GetBackGround(),this->CurrentForeGround);
	}

		

	IplImage*  _BackGroundModeler::GetForeground()
	{
		return this->CurrentForeGround;
	}


	IplImage*  _BackGroundModeler::Filter(IplImage* img)
	{
		CvMat *filter = cvCreateMatHeader(3,3,CV_32FC1);
		cvSetData(filter,f1,filter->step);
		CvMat *filter1 = cvCreateMatHeader(3,3,CV_32FC1);
		cvSetData(filter1,f0,filter1->step);
		
		cvFilter2D(img,tmpfilt1,filter);
		cvFilter2D(img,tmpfilt2,filter1);
		cvAdd(tmpfilt1,tmpfilt2,img);
		
		
		cvReleaseMat(&filter);
		cvReleaseMat(&filter1);
		return img;
	}


	IplImage*  _BackGroundModeler::BFilter(IplImage* img)
	{
		CvMat *filter = cvCreateMatHeader(3,3,CV_32FC1);
		cvSetData(filter,f1,filter->step);
		CvMat *filter1 = cvCreateMatHeader(3,3,CV_32FC1);
		cvSetData(filter1,f0,filter1->step);

		cvFilter2D(img,tmpbfilt1,filter);
		cvFilter2D(img,tmpbfilt2,filter1);
		cvAdd(tmpbfilt2,tmpbfilt1,img);
		
		
		cvReleaseMat(&filter);
		cvReleaseMat(&filter1);

		return img;
	}

}
