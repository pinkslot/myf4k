// Class for managing motion vectors
// TODO: optimize directionToColor() so that pow() is executed only once

#include "motion_vector_utils.h"
#include <cmath>
// Debug includes and usings
#include <iostream>
using namespace std;
	
void MotionVectorUtils::hueToRGB(double h, uchar *r, uchar *g, uchar *b)
{
	Mat m(1, 1, CV_32FC3);
	m.at<Vec3f>(0,0) = Vec3f(h, 1, 1);
	Mat m2;
	cvtColor(m, m2, CV_HSV2BGR);
	Vec3f bgr = m2.at<Vec3f>(0,0);
	*r = bgr[2]*255;
	*g = bgr[1]*255;
	*b = bgr[0]*255;
}

Mat MotionVectorUtils::motionVectorToImage(const Mat& mv, int min_movement, bool draw_directions)
{
	// Check motion vector type
	CV_Assert(mv.type() == CV_64FC4);
	// Compute image width, according to whether we want to draw the direction map
	int direction_map_width = 200;
	int img_width = mv.cols + (draw_directions ? direction_map_width : 0);
	Mat img = Mat::zeros(mv.rows, img_width, CV_8UC3);
	for(int i=0; i<mv.rows; i++)
	{
		for(int j=0; j<mv.cols; j++)
		{
			// Read pixel's vector
			Vec4d v = mv.at<Vec4d>(i,j);
			// Check pixel's movement
			if(v[0] > min_movement)
			{
				uchar r, g, b;
				hueToRGB(v[1], &r, &g, &b);
				img.at<Vec3b>(i,j) = Vec3b(b, g, r);
			}
			else
			{
				// Set pixel to black
				img.at<Vec3b>(i,j) = Vec3b(0,0,0);
			}
		}
	}
	// If needed, draw directions
	if(draw_directions)
	{
		// New map, much cooler
		// Compute center coordinate
		Point2f center(mv.cols + direction_map_width/2, mv.rows/2);
		// Define "corona circolare" from center
		int min_dist = 30;
		int max_dist = 35;
		// Check points (too difficult to explain here...)
		for(int x=center.x-max_dist; x<center.x+max_dist; x++)
		{
			for(int y=center.y-max_dist; y<center.y+max_dist; y++)
			{
				// Compute distance between point and center
				double distance = sqrt((center.x - x)*(center.x-x) + (center.y - y)*(center.y - y));
				// Check distance
				if(distance < max_dist && distance > min_dist)
				{
					// Compute angle
					double angle = fastAtan2(-y + center.y, x - center.x);
					// Convert to degrees
					//angle = angle*180/M_PI;
					// Compute HSV values
					//double s = 1.0;
					//double v = 1.0;
					//double h = angle;
					// Convert to RGB
					uchar r, g, b;
					//HSVToRGB(h, s, v, &r, &g, &b);
					//cout << "(" << x << ", " << y << ") - (" << center.x << ", " << center.y << "), " << h << ", " << s << ", " << v << " -> " << (int) r << ", " << (int) g << ", " << (int) b << endl;
					//img.at<Vec3b>(y,x) = Vec3b(b, g, r);
					//Mat m(1, 1, CV_8UC3);
					//m.at<Vec3b>(0,0) = Vec3b((uchar) h/2, (uchar) s*255, (uchar) v*255);
					//Mat m2;
					//cvtColor(m, m2, CV_HSV2BGR);
					//Vec3b rgb = m2.at<Vec3b>(0,0);
					//img.at<Vec3b>(y,x) = Vec3b(rgb[0], rgb[1], rgb[2]);
					hueToRGB(angle, &r, &g, &b);
					img.at<Vec3b>(y,x) = Vec3b(b, g, r);
					//cout << h << " -> " << (int)r << ", " << (int)g << ", " << (int)b << endl;
					//cout << "--------------" << endl;
				}
			}
		}

	}
	// Return image
	return img;
}
	
// Compute modulus and phase from delta x and delta y
// - mv: motion vector
void MotionVectorUtils::compute01From23(Mat& mv)
{
	// Go through all elements
	for(int i=0; i<mv.rows; i++)
	{
		for(int j=0; j<mv.cols; j++)
		{
			// Read element
			Vec4d& elem = mv.at<Vec4d>(i,j);
			// Read delta_x and delta_y
			double delta_x = elem[2];
			double delta_y = elem[3];
			// Compute modulus and phase
			double modulus = sqrt(delta_x*delta_x + delta_y*delta_y);
			double phase = fastAtan2(-delta_y, delta_x);
			// Set values
			elem[0] = modulus;
			elem[1] = phase;
		}
	}
}
	
/*void MotionVectorUtils::drawArrow(Mat& img, const Point2f& center, float angle, const Scalar& color)
{
	int length = 30;
	int wing = length/4;
	double cosine = cos(angle);
	double sine = sin(angle);
	Point2f end(center.x + length*cosine, center.y - length*sine);
	//Point2f end_w1(end.x - wing*cos(45*M_PI/180 - angle), end.y - wing*sin(45*M_PI/180 - angle));
	//Point2f end_w2(end.x - wing*sin(45*M_PI/180 - angle), end.y + wing*cos(45*M_PI/180 - angle));
	Point2f end_w1(end.x - wing*cos( 45*M_PI/180 - angle), end.y - wing*sin( 45*M_PI/180 - angle));
	Point2f end_w2(end.x - wing*sin( 45*M_PI/180 - angle), end.y + wing*cos( 45*M_PI/180 - angle));
	line(img, center, end, color, 2);
	line(img, end, end_w1, color, 2);
	line(img, end, end_w2, color, 2);
}*/
