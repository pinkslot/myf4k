// Base class which represents an object being tracked.
// Contains methods for accessing information about where the object has been detected.

#include "tracked_object.h"
#include <opencv_utils.h>

namespace alg
{
		
	// Counter for ids
	long int TrackedObject::next_id = 0;

	// Add detection; may force to overwrite a previous detection
	void TrackedObject::addDetection(int frame_num, DetectedObject& detected_object, bool ignore_frame_assigned, bool ignore_detection_assigned)
	{
		// Check frame already assigned
		if(!ignore_frame_assigned && detected_frames.size() > 0 && detected_frames.back() == frame_num)
		{
			// Ignore this region
			return;
		}
		// Check detection already assigned
		if(!ignore_detection_assigned && detected_object.isAssigned())
		{
			// Ignore this region
			return;
		}
		// Save the frame number and detection in region in the appropriate vectors
		detected_frames.push_back(frame_num);
		cvb::CvBlob copy;
		cvCloneBlob(detected_object.region(), copy);
		detected_regions.push_back(copy);
		// Notify assignment
		detected_object.notifyAssigned();
	}

	// Add detection; may force to overwrite a previous detection
	void TrackedObject::addDetection(int frame_num, const cvb::CvBlob& blob, bool ignore_frame_assigned)
	{
		// Check frame already assigned
		if(!ignore_frame_assigned && detected_frames.size() > 0 && detected_frames.back() == frame_num)
		{
			// Ignore this region
			return;
		}
		// Save the frame number and detection in region in the appropriate vectors
		detected_frames.push_back(frame_num);
		cvb::CvBlob copy;
		cvCloneBlob(blob, copy);
		detected_regions.push_back(copy);
	}

	// Draw object contour on a frame
	// - frame: image to write
	// - frame_num: number of current frame; don't draw the object if it doesn't appear there
	// - color: contour color
	void TrackedObject::drawContour(Mat& frame, int frame_num, Scalar color = CV_RGB(255,0,0)) const
	{
		if(lastAppearance() == frame_num)
		{
			drawBlobContour(frame, currentRegion(), color);
		}
	}
}
