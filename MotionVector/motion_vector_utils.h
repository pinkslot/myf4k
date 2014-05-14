// Class for managing motion vectors

#ifndef MOTION_VECTOR_UTILS_H
#define MOTION_VECTOR_UTILS_H

#include <cv.h>

using namespace cv;

class MotionVectorUtils
{
	//static Vec3b directionToColor(int, int);
	//static int angleToDirection(float, int);
	//static void drawArrow(Mat&, const Point2f&, float, const Scalar&);
	static void hueToRGB(double h, uchar *r, uchar *g, uchar *b);

	public:

	// Create an image which highlights motion points and colors them according to the angle with the x axis
	// - mv: motion vector, as computed by the MotionVector class (CV_64FC4, channel 0: modulus, channel 1: angle with the x axis)
	// - min_movement: highlight only points having modulus greater than this value
	// - draw_direction_map: if true, also draw a set of colored arrows to explain what colors mean
	static Mat motionVectorToImage(const Mat& mv, int min_movement, bool draw_direction_map = true);

	// Compute modulus and phase from delta x and delta y
	// - mv: motion vector
	static void compute01From23(Mat& mv);
};

#endif
