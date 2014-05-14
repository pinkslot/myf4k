// Manually-drawn detections
#ifndef HAND_DETECTION_H
#define HAND_DETECTION_H

#include <vector>
#include "detection.h"
#include <cvblob.h>
#include <cv.h>

namespace alg
{

	class HandDetection : public Detection
	{
		private:

		// Path to the XML file
		//string file_path;
		// Counter for the current frame
		//int frame_counter;

		public:
		

		inline std::string name() const { return "hand"; }
		inline std::string description() const { return "Manually-drawn detections"; }
		inline std::string version() const { return "1.0"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
		
		// Constructor
		HandDetection();

		// Destructor, for memory deallocation
		~HandDetection() {}

		// Analyze next frame and return objects:
		// - frame: next input frame, on which to run the detection algorithm
		// - blobs: list of blobs;
		void nextFrame(const IplImage* frame, cvb::CvBlobs& blobs);
	};

}

#endif
