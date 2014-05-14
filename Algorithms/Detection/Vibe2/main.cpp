#include <cvblob.h>
#include "vibe2.h"
#include <cv.h>

using namespace cv;

int main()
{
	ViBE2 v;
	IplImage* img = NULL;
	cvb::CvBlobs blobs;
	v.nextFrame(img, blobs);
}
