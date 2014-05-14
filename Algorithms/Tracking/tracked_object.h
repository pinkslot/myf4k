// Base class which represents an object being tracked.
// Contains methods for accessing information about where the object has been detected.

#ifndef TRACKED_OBJECT_H
#define TRACKED_OBJECT_H

#include <cv.h>
#include <vector>
#include <cvblob.h>
#include "detected_object.h"

namespace alg
{

	class TrackedObject
	{
		protected:

		// Counter for ids
		static long int next_id;
		
		// List of frames in which the object was detected
		std::vector<int> detected_frames;

		// List of detection regions, with positions in the vector corresponding to detectedFrames
		std::vector<cvb::CvBlob> detected_regions;

		public:

		// An identification number for the current object
		long int id;

		// Constructor; sets id
		inline TrackedObject() { id = next_id++; }

		// Virtual destructor
		virtual ~TrackedObject() {}

		// Add detection; may force to overwrite a previous detection
		virtual void addDetection(int frame_num, DetectedObject& detected_object, bool ignore_frame_assigned = false, bool ignore_detection_assigned = false);
		virtual void addDetection(int frame_num, const cvb::CvBlob& blob, bool ignore_frame_assigned = false);
		
		// Returns the last region where the object was identified
		inline const cvb::CvBlob& currentRegion() const { return detected_regions.back(); }
		inline cvb::CvBlob& currentRegion() { return detected_regions.back(); }

		// Returns a reference to the list of frames in which the object appeared
		inline const std::vector<int>& frameList() const { return detected_frames; }
		inline std::vector<int>& frameList() { return detected_frames; }
		
		// Returns a reference to the list of regions in which the object has been detected
		inline const vector<cvb::CvBlob>& regionList() const { return detected_regions; }
		inline vector<cvb::CvBlob>& regionList() { return detected_regions; }

		// Get the last frame number in which the object has been found
		inline int lastAppearance() const { return detected_frames.back(); }

		// Draw object contour on a frame
		// - frame: image to write
		// - frame_num: number of current frame; don't draw the object if it doesn't appear there
		// - color: contour color
		void drawContour(cv::Mat& frame, int frame_num, cv::Scalar color) const;
	};

}

#endif
