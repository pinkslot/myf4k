// Test application

#include <cv.h>
#include <highgui.h>
#include <iostream>
#include <shell_utils.h>
#include <string>
#include "lk_motion_vector.h"
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

int dir_from_angle(float angle, int num_directions)
{
	float dir_step = 360/num_directions;
	int direction = -1;
	float curr_diff = 360;
	for(int dir=0; dir<=num_directions; dir++)
	{
		float arg_dir = dir_step*dir;
		float diff = abs(arg_dir - angle);
		if(diff < curr_diff)
		{
			curr_diff = diff;
			direction = dir;
		}
	}
	// Check if direction is 360° and set it to 0°
	if(direction == num_directions)
		direction = 0;
	return direction;
}

int main(int argc, char** argv)
{
	Mat im = imread(argv[1]);
	cout << "channels rgb: " << im.channels() << endl;
	Mat gs;
	cvtColor(im, gs, CV_BGR2GRAY);
	cout << "channels gs: " << gs.channels() << endl;
	int num_dir = 4;
	int num_steps = 2;
	while(pow(num_steps,3) < num_dir)
	{
		num_steps++;
	}
	int color_step = 255 / (num_steps-1);
	cout << "num steps = " << num_steps << endl;
	cout << "dir step = " << 360/num_dir << endl;
	cout << "color step = " << color_step << endl;
	for(float a=0; a<360; a += 20)
	{
		int c1, c2, c3;
		int dir = dir_from_angle(a,num_dir);
		get_color_components(dir, num_steps, &c1, &c2, &c3);
		cout << a << "\t= " << dir << "\t(" << c1 << ", " << c2 << ", " << c3 << ")" << " = (" << color_step*c1 << ", " << color_step*c2 << ", " << color_step*c3 << ")" << endl;
	}
}
