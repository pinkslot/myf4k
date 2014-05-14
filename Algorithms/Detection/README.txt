Classes:
- Detection: abstract class which defines methods for the implementation of a motion detection algorithm.

Files:
- detection.h: header file for the Detection interface
- detection_template.h: example header file to help with the creation of a new Detection implementation
- detection_template.cpp: example implementation file to help with the creation of a new Detection implementation

Dependencies:
- SWComponent
- CvBlob

---------------

Last update:	Simone, 05-04-2011

Changelog:

05-05-2011 v0.1.0
- Now using CvBlobs in the Detection interface.

22-03-2011 v0.0.3
- Masks are properly computed.

18-03-2011 v0.0.2
- Fixed memory leaks (hopefully).

16-03-2011 v0.0.1
- First version of all classes
