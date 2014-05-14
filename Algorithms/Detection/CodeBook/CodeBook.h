#ifndef ALG_CB_H
#define ALG_CB_H
#define NCHANNELS 3
#define TRAIN_FRAMES 100
#include "detection.h"
#include "cv.h"
#include "highgui.h"
#include "cvblob.h"
#include "cxcore.h"
#include <string.h>
#include <stdlib.h>
#include "opencv2/core/core.hpp"
#include "opencv2/video/background_segm.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/legacy/legacy.hpp"

namespace alg
{
	class CodeBook:public Detection
	{
		private:
			int n, nframes;
			CvBGCodeBookModel* model;
			int nframesToLearnBG;
			bool ch[NCHANNELS];
			IplImage* rawImage, *yuvImage, *tmp;
		        IplImage *ImaskCodeBook,*ImaskCodeBookCC;

			bool _init;
		public:
			CodeBook(void);
			~CodeBook(void);
			cv::Mat nextFrame(Context& context);
			inline string name() const { return "cb"; }
			inline string description() const { return "Codebook"; }
			inline string version() const { return "1.0"; }
			inline int executionTime() const { return 0; }
			inline int ram() const { return 0; }
			inline void init(vector<IplImage*> imgArray){}
			inline void init(IplImage* img){}
	};
}
#endif
