Classes:
- Tracker: abstract class which defines methods for processing a new frame (nextFrame()) and accessing tracking results (getTrackedObjects())
- TrackedObject: basic class which represents an object being tracked; contains methods for accessing the number of frames and the positions where the object has been found.
- DetectedObject: basic class which represents an object which has been detected in the current frame and has to be either assigned to an existing tracked object or used to create a new
  tracked object.

Files:
- tracker.h: header file for the Tracker interface
- tracked_object.h: header file for the TrackedObject class
- detected_object.h: header file for the DetectedObject class

Dependencies: none

---------------

Last update:	Simone, 18-03-2011

Changelog:

07-04-2011 v0.1.0
- Completed conversion to CvBlob.

18-03-2011 v0.0.2
- Fixed bug in the access to tracked objects.
- Set initial version 0.0.2.

08-03-2011
- Added DetectedObject.

28-02-2011
- First version.
