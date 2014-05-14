// Class for the computation of the motion vector over a set of images using the Lucas-Kanade method

#include "lk_motion_vector.h"
#include <shell_utils.h>
#include <vector>
#include <iostream>

using namespace std;
	
// Select all points in the image
Mat LKMotionVector::addFrame(const Mat& frame)
{
	// Prepare point set
	vector<Point2f> prev_points;
	for(int i=0; i<frame.rows; i++)
	{
		for(int j=0; j<frame.cols; j++)
		{
			prev_points.push_back(Point2f(j,i));
		}
	}
	// Call generic addFrame method
	return addFrame(frame, prev_points);
}

// Select only points which are set to 1 in the given binary mask
Mat LKMotionVector::addFrame(const Mat& frame, const Mat& mask)
{
	// Prepare point set
	vector<Point2f> prev_points;
	for(int i=0; i<frame.rows; i++)
	{
		for(int j=0; j<frame.cols; j++)
		{
			if(mask.at<uchar>(i,j) != 0)
			{
				prev_points.push_back(Point2f(j,i));
			}
		}
	}
	// Call generic addFrame method
	return addFrame(frame, prev_points);
}

// Select only points belonging to the given rectangles
Mat LKMotionVector::addFrame(const Mat& frame, const vector<Rect>& rects)
{
	// Prepare point set
	vector<Point2f> prev_points;
	for(vector<Rect>::const_iterator it = rects.begin(); it != rects.end(); it++)
	{
		const Rect& rect = *it;
		for(int x=rect.x; x<rect.x+rect.width-1; x++)
		{
			for(int y=rect.y; y<rect.y+rect.height-1; y++)
			{
				prev_points.push_back(Point2f(x,y));
			}
		}
	}
	// Call generic addFrame method
	return addFrame(frame, prev_points);
}

// Select only points belonging to the given rectangle
Mat LKMotionVector::addFrame(const Mat& frame, const Rect& rect)
{
	// Prepare point set
	vector<Point2f> prev_points;
	for(int x=rect.x; x<rect.x+rect.width-1; x++)
	{
		for(int y=rect.y; y<rect.y+rect.height-1; y++)
		{
			prev_points.push_back(Point2f(x,y));
		}
	}
	// Call generic addFrame method
	return addFrame(frame, prev_points);
}

// Select only specified points
Mat LKMotionVector::addFrame(const Mat& frame, const vector<Point2f>& prev_points)
{
	// Check if this is the first frame or not
	if(! buffer_valid)
	{
		// Set this frame as the "next" frame
		next = frame;
		// Set buffer_valid
		buffer_valid = true;
		// Return Mat::zeros().
		return Mat::zeros(next.rows, next.cols, CV_64FC4);
	}
	else
	{
		// Set this as the new "next" frame and the current "next" as the new "priv"
		prev = next;
		next = frame;
		// Check image sizes and types
		if(prev.cols != next.cols || prev.rows != next.rows || prev.type() != next.type())
		{
			cout << ShellUtils::RED << "LKMotionVector::addFrame(): the input images must have the same size and type." << endl << ShellUtils::NORMAL;
			return Mat::zeros(prev.rows, prev.cols, CV_64FC4);
		}
		// Execute OpenCV PyrLK algorithm
		vector<Point2f> next_points;
		vector<uchar> status;
		vector<float> err;
		calcOpticalFlowPyrLK(prev, next, prev_points, next_points, status, err, Size(window_size, window_size), max_level, term_criteria);
		// Create output matrix
		Mat motion_vector = Mat::zeros(prev.rows, prev.cols, CV_64FC4);
		for(unsigned int i=0; i<prev_points.size(); i++)
		{
			const Point2f& prev_pt = prev_points[i];
			const Point2f& next_pt = next_points[i];
			// Compute modulus and phase
			double modulus = sqrt((prev_pt.x - next_pt.x)*(prev_pt.x - next_pt.x) + (prev_pt.y - next_pt.y)*(prev_pt.y - next_pt.y));
			double phase = fastAtan2(-next_pt.y + prev_pt.y, next_pt.x - prev_pt.x);
			// Compute x and y offsets
			double delta_x = next_pt.x - prev_pt.x;
			double delta_y = next_pt.y - prev_pt.y;
			// Set corresponding pixel in the motion vector
			motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[0] = modulus;
			motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[1] = phase;
			motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[2] = delta_x;
			motion_vector.at<Vec4d>(prev_pt.y, prev_pt.x)[3] = delta_y;
		}
		// Return the computed motion vector
		return motion_vector;
	}
}
