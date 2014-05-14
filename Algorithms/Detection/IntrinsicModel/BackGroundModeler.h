#ifndef ALG_IM_BACKGROUND_H
#define ALG_IM_BACKGROUND_H

#include "cxcore.h"
#include "highgui.h"
#include "cv.h"
#include "cvblob.h"


#include <vector>

using namespace std;

namespace alg
{

	//#define _LINUX_
	//#define _DEBUG_ 
	struct _LinkedListNode{
		_LinkedListNode *next,*previous;
		IplImage* img;
		CvSize getSize();
	};


	typedef struct _LinkedListNode *IplImageNode;

	class _BackGroundModeler {
		private:

		int background_window;

	public:
		_BackGroundModeler(vector<IplImage*> l, int bg_win);
		
		IplImageNode current,First,Last;
		
		IplImage* getNext();
		IplImage* getPrevious();
		IplImage* getCurrent();

		IplImage* UpdateBackGround();

		IplImage* GetForeground();


		IplImage* GetBackGround();
		IplImage* CurrentBackGround;
		IplImage* CurrentForeGround;

		void UpdateForeground(IplImage* img);
		void UpdateForeground();
		void add(IplImage* img);
		IplImage* GetCTN();

		IplImage* FloatImage(IplImage* img);
		IplImage* BFloatImage(IplImage* img);

		IplImage* LogImage(IplImage* img);
		IplImage* BLogImage(IplImage* img);

		IplImage* ExpImage(IplImage* img);
		IplImage* BExpImage(IplImage* img);
		
		IplImage* Filter(IplImage* img);
		IplImage* BFilter(IplImage* img);
		

		IplImage* tmpfloat;
		IplImage* tmpbfloat;

		IplImage* tmpexp;
		IplImage* tmpbexp;

		IplImage* tmplog;
		IplImage* tmpblog;

		IplImage* tmpfilt1;
		IplImage* tmpfilt2;

		IplImage* tmpbfilt1;
		IplImage* tmpbfilt2;

		IplImage* tmp8u;
		int size;

		int initImageCount();

		int width;
		int height;
		bool hasNext();
		bool hasPrevious();
		bool hasNewItems,BackGroundReady;
		bool updating,reading;
		
		void setCurrentToFirst();
		void setCurrentToLast();

	public:
		int framecount;
		CvMemStorage* storage;
		
	};

	typedef class _BackGroundModeler *BackGroundModeler;
	int comparefloat (const void * a, const void * b);

}

#endif
