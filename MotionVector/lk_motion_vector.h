// Class for the computation of the motion vector over a set of images using the Lucas-Kanade method
// TODO: try to remove the "full" flag

#ifndef LK_MOTION_VECTOR_H
#define LK_MOTION_VECTOR_H

#include "motion_vector.h"

class LKMotionVector : public MotionVector
{
	// Buffers for the two most recent frames on which to compute the optical flow
	Mat prev, next;
	// Flag which tells if we already have added enough images to the buffer
	bool buffer_valid;

	public:

	// Algorithm parameters:
	// - Size of the search window at each pyramid level
	int window_size;
	// - 0-based pyramid level number
	int max_level;
	// - Algorithm termination criteria
	TermCriteria term_criteria;
	
	// Constructor; initialize parameters
	inline LKMotionVector() :
		buffer_valid(false), window_size(15), max_level(3), term_criteria(TermCriteria::COUNT + TermCriteria::EPS, 5, 0.01) {}

	// Update the frame sequence with the current frame and return the current motion vector for the whole image
	// - frame: new frame
	virtual Mat addFrame(const Mat& frame);
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified rectangle
	// - frame: new frame
	// - rect: rectangle in which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const Rect& rect);
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified rectangles
	// - frame: new frame
	// - rect_list: list of rectangles in which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const vector<Rect>& rect_list);
	
	// Update the frame sequence with the current frame and return the current motion vector for the specified points
	// - frame: new frame
	// - point_list: list of points for which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const vector<Point2f>& point_list);
	
	// Update the frame sequence with the current frame and return the current motion vector for the points whose coordinates are set to 1 in the mask matrix
	// - frame: new frame
	// - mask: matrix where 1's indicate points for which we want to compute the vector
	virtual Mat addFrame(const Mat& frame, const Mat& mask);

};

#endif
