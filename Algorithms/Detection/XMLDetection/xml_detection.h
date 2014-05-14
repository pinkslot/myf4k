// Provides detection results from an XML file

#ifndef XML_DETECTION_H
#define XML_DETECTION_H

#include <vector>
#include "detection.h"
#include <cvblob.h>
#include <cv.h>
#include <xmltdresults.h>

using namespace std;
using namespace cv;

namespace alg
{

	class XMLDetection : public Detection
	{
		private:

		// Path to the XML file
		//string file_path;
		// Counter for the current frame
		int frame_counter;
		// XML file reference
		XMLTDResults xml_results;
		// Results from the XML file
		pr::Results results;
		// Variables for managing frame rate
		int last_ffps_frame;

		public:
		

		inline string name() const { return "xml"; }
		inline string description() const { return "Data from XML file"; }
		inline string version() const { return "1.1"; }
		inline int executionTime() const { return 0; }
		inline int ram() const { return 0; }
		
		// Constructor
		XMLDetection();

		// Read XML data
		void readFile(string f);

		// Destructor, for memory deallocation
		~XMLDetection() {}

		// Analyze next frame and return objects:
		// - frame: next input frame, on which to run the detection algorithm
		// - blobs: list of blobs;
		cv::Mat nextFrame(Context& context);
	};

}

#endif
