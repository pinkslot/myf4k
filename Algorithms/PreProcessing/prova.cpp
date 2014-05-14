// Utility to add video and camera data to the database.
// Usage: add_camera_video_to_db <site> <video num> <date> <time> <video file>

// Common includes
#include <iostream>
// OpenCV includes
#include <cv.h>
#include <log.h>
#include <highgui.h>
#include <tclap/CmdLine.h>
#include "contrast_stretching.h"
#include <my_exception.h>

using namespace TCLAP;
using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	// Define variables for command-line parameters
	string video_path;
	// Initialize log
	Log::setDebugLogStream(cout);
	Log::setInfoLogStream(cout);
	Log::setWarningLogStream(cout);
	Log::setErrorLogStream(cerr);
	// Define command-line parameters for TCLAP
	try
	{
		// Define cmd object
		CmdLine cmd("Usage: ", ' ', "0.0.1");
		// Define video file path
		UnlabeledValueArg<string> videoPathArg("video-path", "Video path", true, "", "path", cmd);
		// Parse actual arguments
		cmd.parse(argc, argv);
		// Read easy values
		video_path = videoPathArg.getValue();
		// Check dependencies for complicated stuff
		// ...nothing to be done
	}
	catch (ArgException& e)
	{
		Log::error() << "Error while parsing command line arguments. " << e.error() << " for arg " << e.argId() << endl;
		exit(1);
	}
	catch (...)
	{
		Log::error() << "Error while parsing command line arguments. " << endl;// << e.error() << " for arg " << e.argId() << endl;
		exit(1);
	}
	// Open video
	VideoCapture cap(video_path);
	// Create video object
	int frame_rate = cap.get(CV_CAP_PROP_FPS);
	int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
	int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
	cout << frame_width << "x" << frame_height << "@" << frame_rate << endl;
	int frame_depth = -1; // FIXME: get from command line
	//int encoding = "unknown"; // FIXME: get from command line
	try
	{
		// Print stuff
		ContrastStretching preproc;
		Mat frame;
		int frame_num = 0;
		while(true)
		{
			cout << "Frame: " << frame_num << endl;
			cap >> frame;
			imshow("frame", frame);
			Mat preproc_frame = preproc.nextFrame(frame);
			imshow("preproc_frame", preproc_frame);
			//Mat resized;
			//resize(frame, resized, Size(320,240));
			//imshow("bbb", resized);
			char c = waitKey(0);
			if(c == 115)
			{
				// Save
				stringstream ss;
				ss << "frame_" << frame_num << ".png";
				imwrite(ss.str(), frame);
			}
			frame_num++;
		}
	}
	catch(MyException& e)
	{
		cout << e.what() << endl;
	}
	catch(exception e)
	{
		cout << e.what() << endl;
	}
	catch(...)
	{
		cout << "unknown exception" << endl;
	}
}
