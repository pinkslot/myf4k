// Class which represents and object which has been detected but not yet classified (i.e. assigned to an existing object or identified as a new one).

#ifndef DETECTED_OBJECT_H
#define DETECTED_OBJECT_H

#include <cv.h>

using namespace cv;

namespace alg
{

	class DetectedObject
	{
		protected:
		
		// Coordinates of the rectangle containing the object
		cvb::CvBlob region_;

		// Flag which tells if the region has been assigned to an existing object or not
		bool assigned_;

		public:

		// Constructor
		DetectedObject() : assigned_(false) {}

		// Accessor method for region
		inline cvb::CvBlob& region() { return region_; };

		// Notify that the region has been assigned to a tracked object
		inline void notifyAssigned() { assigned_ = true; }

		// Check if the region has been assigned
		inline bool isAssigned() { return assigned_; }
	};

}

#endif
