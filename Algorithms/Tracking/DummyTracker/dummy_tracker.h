// Doesn't actually track anything, assigns to each detection a different id

#ifndef DUMMY_TRACKER_H
#define DUMMY_TRACKER_H

#include <cv.h>
#include <vector>
#include <tracker.h>
#include <log.h>

using namespace cv;

namespace alg
{

	class DummyTracker : public Tracker
	{
		private:

		protected:

		public:

		inline string name() const { return "dummy"; }
		inline string description() const { return "Assigns to each detection a new tracked object"; }
		inline string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }

		// Default constructor
		DummyTracker();
		
		// Destructor; has to free tracked objects
		~DummyTracker();

		// Read next frame, frame number, object blobs and perform tracking.
		// The input image type is supposed to be CV_8UC3.
		void nextFrame(const Mat&, int, const cvb::CvBlobs& blobs);
		
		// Adapt tracking parameters to video properties
		// TODO
		void adaptToVideo(VideoCapture& video_cap) {}

		// Draw objects' tracks.
		// The input matrix should be the frame on which to draw.
		//void drawTracks(Mat&);
	};

}

#endif
