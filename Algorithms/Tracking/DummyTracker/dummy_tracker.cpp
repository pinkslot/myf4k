// Doesn't actually track anything, assigns to each detection a different id

#include "dummy_tracker.h"
#include <opencv_utils.h>

// Debug includes
#include <iostream>
#include <highgui.h>
#include <sstream>

using namespace std;

namespace alg
{

	// Default constructor
	DummyTracker::DummyTracker()
	{
		// Instantiate internal objects for tracking
		// ...
	}

	// Destructore; has to free tracked objects
	DummyTracker::~DummyTracker()
	{
		// Free currently tracked objects
		for(vector<TrackedObject*>::iterator it = tracked_objects.begin(); it != tracked_objects.end(); it++)
		{
			delete *it;
		}
		// Free previously tracked objects
		for(vector<TrackedObject*>::iterator it = lost_objects.begin(); it != lost_objects.end(); it++)
		{
			delete *it;
		}
	}
		
	// Read next frame, frame number, object blogs binary masks and perform tracking.
	// The input image type is supposed to be CV_8UC3.
	void DummyTracker::nextFrame(const Mat& frame, int frame_number, const cvb::CvBlobs& blobs)
	{
		// Build list of detected objects
		vector<DetectedObject> detected_objects;
		for(cvb::CvBlobs::const_iterator it = blobs.begin(); it != blobs.end(); it++)
		{
			// Get blob
			cvb::CvBlob blob;
			cvCloneBlob(*(it->second), blob);
			// Define detected object
			DetectedObject detected_object;
			detected_object.region() = blob;
			// Add to list
			detected_objects.push_back(detected_object);
		}
		// Create a copy of the list of currently tracked object, and clear the original list (we'll add objects later)
		vector<TrackedObject*> tracked_objects_backup(tracked_objects);
		tracked_objects.clear();
		// Go through all currently existing tracked objects and check if they have been found in the current frame; if not, remove them
		for(vector<TrackedObject*>::iterator it = tracked_objects_backup.begin(); it != tracked_objects_backup.end(); it++)
		{
			// Check if the object has been found in the current frame
			if((*it)->lastAppearance() != frame_number)
			{
				// Add it to the lost object list
				lost_objects.push_back(*it);
			}
			else
			{
				// Add the object to the list
				tracked_objects.push_back(*it);
			}
		}
		// Go through all unassigned objects
		for(vector<DetectedObject>::iterator it = detected_objects.begin(); it != detected_objects.end(); it++)
		{
			if(!it->isAssigned())
			{
				// Get detected object
				DetectedObject& detected_object = *it;
				// Create new tracked object
				TrackedObject* tracked_object = new TrackedObject();
				// Add detection
				tracked_object->addDetection(frame_number, detected_object);
				// Add to list
				tracked_objects.push_back(tracked_object);
			}
		}
		// I guess we're done here...
		//Log::info() << "Tracked objects: ["; for(int i=0; i<tracked_objects.size(); i++) Log::info(0) << tracked_objects[i]->id << " "; Log::info(0) << "]" << endl;
		//Log::info() << "Lost objects: ["; for(int i=0; i<lost_objects.size(); i++) Log::info(0) << lost_objects[i]->id << " "; Log::info(0) << "]" << endl;
	}

}
