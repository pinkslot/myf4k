#ifndef ALG_IM_H
#define ALG_IM_H

#include "detection.h"
#include "cv.h"
#include "highgui.h"
#include "cvblob.h"
#include "BackGroundModeler.h"
#include <string.h>
#include <stdlib.h>

using namespace cvb;

namespace alg
{

	class IModel :public Detection
	{
		public:
	
			IModel(void) : Detection()
		{
			this->imageCount=0;
			// Setup parameters
			parameters.add<int>("background_window", 15);
			parameters.add<int>("sample_period", 1);
			parameters.add<float>("threshold", 0.4);
		};

		~IModel(void);
		cv::Mat nextFrame(Context& context);
		int initImageCount();
		void init(vector<IplImage*> imgArray);
		void init(IplImage* img);
		IplImage* GetInGrayScale(IplImage* img);
		
		inline string name() const { return "im"; }
		inline string description() const { return "Intrinsic Model"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
		
		private:
		
		IplImage* FloatImage(const IplImage* img);
		IplImage* ExpImage(IplImage* img);
		vector<IplImage*> initVector;
		int width,height,imageCount;
		IplImage* tmp;
		IplImage* getMask(CvRect bbox,IplImage* img);

		BackGroundModeler l;
		int area;

	};

}

#endif
