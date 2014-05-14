// Test application

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <shell_utils.h>
#include <string>
#include "lk_motion_vector.h"
#include "motion_vector_utils.h"
#include <time_utils.h>

using namespace cv;
using namespace std;

void get_color_components(int num, int num_steps, int* comp_1, int* comp_2, int* comp_3)
{
	// Get first component
	*comp_1 = num % num_steps;
	// Get second component
	num = num/num_steps;
	*comp_2 = num % num_steps;
	// Get this component
	num = num/num_steps;
	*comp_3 = num % num_steps;
}

Mat mv2img(const Mat& mv, int min_movement, int num_directions)
{
	// Define direction color step and angle step
	int num_steps_per_channel = 2;
	while(pow(num_steps_per_channel,3) < num_directions)
	{
		num_steps_per_channel++;
	}
	int color_step = 255 / (num_steps_per_channel-1);
	float dir_step = 360/num_directions;
	// Create image
	Mat arg_img = Mat::zeros(mv.rows, mv.cols, CV_8UC3);
	for(int i=0; i<mv.rows; i++)
	{
		for(int j=0; j<mv.cols; j++)
		{
			// Compute direction
			if(mv.at<Vec2d>(i,j)[0] > min_movement)
			{
				// Directions: 0: 0°, 1: N°, 2: 2*N°, etc
				int direction = -1;
				float curr_diff = 360;
				for(int dir=0; dir<=num_directions; dir++)
				{
					float arg_dir = dir_step*dir;
					float diff = abs(arg_dir - mv.at<Vec2d>(i,j)[1]);
					if(diff < curr_diff)
					{
						curr_diff = diff;
						direction = dir;
					}
				}
				// Check if direction is 360° and set it to 0°
				if(direction == num_directions)
					direction = 0;
				// Set appropriate color
				int comp_1, comp_2, comp_3;
				get_color_components(direction+1, num_steps_per_channel, &comp_1, &comp_2, &comp_3);
				arg_img.at<Vec3b>(i,j) = Vec3b(comp_1*color_step, comp_2*color_step, comp_3*color_step);
			}
			else
			{
				arg_img.at<Vec3b>(i,j) = Vec3b(0,0,0);
			}
		}
	}
	return arg_img;
}

int main(int argc, char** argv)
{
	namedWindow("img_1_win");
	namedWindow("img_2_win");
	namedWindow("mv_win");
	// Read images
	Mat img_2 = imread("frames/frame_578.png");
	Mat img_1 = imread("frames/frame_584.png");
	//Mat img_1_gs, img_2_gs;
	//cvtColor(img_1, img_1_gs, CV_RGB2GRAY);
	//cvtColor(img_2, img_2_gs, CV_RGB2GRAY);
	//imshow("img_1_win", img_1_gs);
	//imshow("img_2_win", img_2_gs);
	// Try different configurations
	for(int w=3; w<16; w += 2)
	{
		for(int m=0; m<1; m++)
		{
			for(double d=0.0; d<=1.0; d += 0.2)
			{
				cout << "w=" << w << ", m=" << m << ", d=" << d << endl;
				LKMotionVector motion;
				motion.window_size = w;
				motion.max_level = m;
				motion.derivative_weight = d;
				TimeUtils::tic();
				Mat mv;
				mv = motion.addFrame(img_1);//, Rect(160, 120, 160, 120));
				mv = motion.addFrame(img_2);//, Rect(160, 120, 160, 120));
				TimeUtils::toc();
				// Create image highlighting pixels which moved by more than n pixels
				int min_movement = 10;
				// Create image with different colors for different movement directions
				Mat mv_img = MotionVectorUtils::motionVectorToImage(mv, min_movement, true);
				stringstream ss;
				ss << "mv_" << w << "_" << m << "_" << d << ".png";
				imwrite(ss.str(), mv_img);
			}
		}
	}
	// Compute motion vector
	/*LKMotionVector motion_vector;
	motion_vector.window_size = 10;
	TimeUtils::tic();
	Mat mv;
	mv = motion_vector.addFrame(img_1_gs);//, Rect(160, 120, 160, 120));
	mv = motion_vector.addFrame(img_2_gs);//, Rect(160, 120, 160, 120));
	TimeUtils::toc();
	// Create image highlighting pixels which moved by more than n pixels
	int min_movement = 10;
	int num_directions = 5;
	// Create image with different colors for different movement directions
	Mat mv_img = MotionVectorUtils::motionVectorToImage(mv, min_movement, num_directions, true);
	// Show motion vector windows
	//imshow("motion_win", mv_img);
	imshow("mv_win", mv_img);
	waitKey(0);*/
	return 0;
}
