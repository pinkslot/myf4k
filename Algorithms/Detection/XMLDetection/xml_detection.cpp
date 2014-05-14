// Provides detection results from an XML file
#include "xml_detection.h"
#include <results.h>
#include <my_exception.h>
#include <opencv_utils.h>
#include <blob_processing.h>
#include <log.h>

using namespace pr;

namespace alg
{

	// Constructor, for variable initialization and parameter setting
	XMLDetection::XMLDetection() : frame_counter(0), last_ffps_frame(-1)
	{
		parameters.add<string>("path", "results.xml");
		parameters.add<float>("fps_ratio", 1);
	}

	void XMLDetection::readFile(string path)
	{
		// Read results
		results = xml_results.readFile(path);
	}

	// Analyze next frame and return objects:
	cv::Mat XMLDetection::nextFrame(Context& context)
	{
		// XML initialization
		static bool first_call = true;
		if(first_call)
		{
			readFile(parameters.get<string>("path"));
			first_call = false;
		}
		// Read fps_ratio
		float fps_ratio = parameters.get<float>("fps_ratio");
		// Counter for blob labels
		int label = 1;
		// Get objects in current frame
		ObjectList objects = results.getObjectsByFrame(frame_counter);
		// Define blobs
		cvb::CvBlobs blobs;
		// For each object, compute the corresponding CvBlob
		for(ObjectList::iterator it = objects.begin(); it != objects.end(); it++)
		{
			// Get object
			Object object = *it;
			// Build CvBlob
			cvb::CvBlob *blob = createBlob(object.getContour());
			// Check blob is valid
			if(!isBlobValid(*blob))
			{
				// Skip
				Log::warning() << "Invalid blob found, skipping." << endl;
				continue;
			}
			// Add to vector
			blobs[label++] = blob;
		}
		// Increment frame counter, taking into account forced frame rate
		last_ffps_frame = (int)(frame_counter*fps_ratio);
		while(last_ffps_frame == (int)(frame_counter*fps_ratio))
		{
			frame_counter++;
		}
		// Build result
		Mat result = rebuildBlobs(blobs, context.frame.cols, context.frame.rows);
		context.motion_output = result;
		return result;
	}

}
