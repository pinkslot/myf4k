Classes:
- MotionVector: abstract class which defines an addFrame() method, which adds an image to the buffer and returns the computed motion vector matrix (CV_64FC2, channel 0: modulus, channel 1:
  angle).
- LKMotionVector: implementation of the MotionVector class which uses the Lucas-Kanade method (as implemented by OpenCV's calcOpticalFlowPyrLK()).
- MotionVectorUtils: utility class for handling motion vector matrices (currently, it provides the motionVectorToImage() method to visualize the motion vector).

Files:
- motion_vector.h: header file for the MotionVector interface
- lk_motion_vector.h: header file for the LKMotionVector class
- lk_motion_vector.cpp: implementation file for the LKMotionVector class
- motion_vector_utils.h: header file for the MotionVectorUtils class
- test_motion.cpp: test file which computes the motion vector between two images
- Makefile: GNU makefile for compilation, generation of the libmotion_vector.a library

Dependencies:
- ShellUtils class for test_motion.cpp

---------------

Last update:	Simone, 18-03-2011

Changelog:

18-03-2011 v0.0.1
- Changed prova.cpp so that it outputs a colored version of the Gabor filter (quite useless, but cool).
- Set version number 0.0.1.

28-02-2011
- First version.
