// Class for the creation of images containing all detected objects in a video

#include "detection_summary.h"
#include <highgui.h>
#include <iostream>
#include <iomanip>
#include <shell_utils.h>
#include <sstream>

using namespace std;
using namespace cv;
	
// Constructor, with optional height and width as input
DetectionSummary::DetectionSummary(int h, int w) :
	counter(0), height(h), width(w), next_x(0), next_y(0), max_row_height(0), first_image_added(false)
{
	current_object_summary = Mat::zeros(h, w, CV_8UC3);
	current_mask_summary = Mat::zeros(h, w, CV_8UC1);
}

DetectionSummary::~DetectionSummary()
{
	if(first_image_added)
	{
		saveCurrentImages();
	}
}

// Save current image and get ready for the next one
void DetectionSummary::saveCurrentImages()
{
	// Write current images
	stringstream object_summary_filename, mask_summary_filename;
	object_summary_filename << setfill('0');
	mask_summary_filename << setfill('0');
	object_summary_filename << prefix << "objects_" << setw(3) << counter << ".png";
	mask_summary_filename << prefix << "masks_" << setw(3) << counter << ".png";
	counter++;
	imwrite(object_summary_filename.str(), current_object_summary);
	imwrite(mask_summary_filename.str(), current_mask_summary);
	// Reset images
	current_object_summary.setTo(Scalar(0,0));
	current_mask_summary.setTo(Scalar(0,0));
	// Reset counters
	max_row_height = 0;
	next_x = 0;
	next_y = 0;
}

// Add a detection to the current output images
void DetectionSummary::addDetection(const Mat& object, const Mat& mask)
{
	// Set flag
	first_image_added = true;
	// Check if the object fits the image
	if(object.rows > current_object_summary.rows || object.cols > current_object_summary.cols)
	{
		cout << ShellUtils::RED << "DetectionSummary::addDetection(): object too large, skipping..." << ShellUtils::NORMAL << endl;
		return;
	}
	// Check if the object and mask sizes are the same
	if(object.size() != mask.size())
	{
		cout << ShellUtils::RED << "DetectionSummary::addDetection(): the object and mask sizes must be the same, skipping..." << ShellUtils::NORMAL << endl;
		return;
	}
	// Convert the input matrices to the correct format (C3 for object, C1 for mask)
	Mat conv_object, conv_mask;
	if(object.channels() != 3)
	{
		cvtColor(object, conv_object, CV_GRAY2BGR);
	}
	else
	{
		conv_object = object.clone();
	}
	if(mask.channels() != 1)
	{
		cvtColor(mask, conv_mask, CV_BGR2GRAY);
	}
	else
	{
		conv_mask = mask.clone();
	}
	// Check if the new object fits in the current image horizontally, and go to a new line if necessary
	if((next_x + conv_object.cols) > width)
	{
		// Move to next row
		next_x = 0;
		next_y += max_row_height;
		max_row_height = 0;
	}
	// Check if the new object fits in the current image vertically, and go to a new image if necessary
	if((next_y + conv_object.rows) > height)
	{
		// Save image and create a new one
		saveCurrentImages();
	}
	// Write object and mask
	for(int x=0; x<conv_object.cols; x++)
	{
		for(int y=0; y<conv_object.rows; y++)
		{
			current_object_summary.at<Vec3b>(y+next_y,x+next_x) = conv_object.at<Vec3b>(y,x);
			current_mask_summary.at<uchar>(y+next_y,x+next_x) = conv_mask.at<uchar>(y,x);
		}
	}
	// Check max_row_height
	if(conv_object.rows > max_row_height)
		max_row_height = conv_object.rows;
	// Increment next_x (it will be checked later)
	next_x += conv_object.cols;
}
