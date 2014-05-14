// Template class for the implementation of a new detection algorithm

#ifndef wave_back_h
#define wave_back_h

#include <vector>
#include "detection.h"
#include "cv.h"
#include "highgui.h"
#include "cvaux.h"
#include "cvblob.h"

#include <time.h>

using namespace std;
using namespace cv;
using namespace cvb;

namespace alg
{

	class wave_back : public Detection
	{
		private:
		
		typedef IplImage* Im;
		int n_frame_bkg, read_frame;
		CvMatND *mat_frame;
		CvMatND *bkg;
		CvMatND *mat_mask;
		CvMatND *mat_norm;
		CvMat *bkg_elem;
		int mat_norm_size;

		public:


		inline string name() const { return "wb"; }
		inline string description() const { return "Wave-back"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
		
		wave_back();

		virtual ~wave_back();

		// Define nextFrame() method, which will be implemented according to the new algorithm
		// Analyze next frame and return objects:
		// - frame: next input frame, on which to run the detection algorithm
		// - bounding_boxes: vector of bounding boxes, one for each object recognized in the frame
		// - masks: vector of binary masks; the size of the i-th mask must be the same size as the i-th bounding box;
		//   the images must be allocated inside the function and the user of the class must free them.
		cv::Mat nextFrame(Context& context);
		void add_to_frame_buffer(IplImage* frame);
		void add_to_mask_buffer(IplImage* frame);

	};

}

#endif
