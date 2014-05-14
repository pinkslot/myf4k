// Abstract class for the computation of the motion vector over a set of images.
// The output matrix of the addFrame methods must be a matrix where each element is a 4D double vector, whose elements are:
// - modulus
// - phase
// - delta_x
// - delta_y

#ifndef MOTION_VECTOR_H
#define MOTION_VECTOR_H

#include <cv.h>
#include <vector>

using namespace cv;
using namespace std;

class MotionVector
{
	public:

	// Update the frame sequence with the current frame and return the current motion vector for the whole image
	// - frame: new frame
	virtual Mat addFrame(const Mat& frame) = 0;
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified rectangle
	// - frame: new frame
	// - rect: rectangle in which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const Rect& rect) = 0;
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified rectangles
	// - frame: new frame
	// - rect_list: list of rectangles in which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const vector<Rect>& rect_list) = 0;
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified points
	// - frame: new frame
	// - point_list: list of points for which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const vector<Point2f>& point_list) = 0;
	
	// Update the frame sequence with the current frame and return the current motion vector for the points whose coordinates are set to 1 in the mask matrix
	// - frame: new frame
	// - mask: matrix where 1's indicate points for which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const Mat& mask) = 0;
	
	// Update the frame sequence with the current frame and return the current motion vector for the whole image
	// - frame: new frame
	inline void addFrame(const Mat& frame, Mat& mv)
	{
		mv = addFrame(frame);
	}
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified rectangle
	// - frame: new frame
	// - rect: rectangle in which we want to compute the vector
	inline void addFrame(const Mat& frame, const Rect& rect, Mat& mv)
	{
		mv = addFrame(frame, rect);
	}
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified rectangles
	// - frame: new frame
	// - rect_list: list of rectangles in which we want to compute the vector
	inline void addFrame(const Mat& frame, const vector<Rect>& rect_list, Mat& mv)
	{
		mv = addFrame(frame, rect_list);
	}
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified points
	// - frame: new frame
	// - point_list: list of points for which we want to compute the vector
	inline void addFrame(const Mat& frame, const vector<Point2f>& point_list, Mat& mv)
	{
		mv = addFrame(frame, point_list);
	}
	
	// Update the frame sequence with the current frame and return the current motion vector for the points whose coordinates are set to 1 in the mask matrix
	// - frame: new frame
	// - mask: matrix where 1's indicate points for which we want to compute the vector
	inline void addFrame(const Mat& frame, const Mat& mask, Mat& mv)
	{
		mv = addFrame(frame, mask);
	}
};

#endif
