// Class for the creation of images containing all detected objects in a video
#ifndef DETECTION_SUMMARY_H
#define DETECTION_SUMMARY_H

#include <cv.h>

class DetectionSummary
{
	private:

	// Counter for the current output images
	int counter;
	// Current image for bounding boxes
	cv::Mat current_object_summary;
	// Current image for masks
	cv::Mat current_mask_summary;
	// Size of the output images
	int height, width;
	// Destination coordinates for the next image
	int next_x, next_y;
	// Variable for storing the maximum height of the images starting at a certain y-coordinate
	int max_row_height;
	// Output image prefix
	std::string prefix;
	// Flag to check if we have added at least one image
	bool first_image_added;

	// Save current image and get ready for the next one
	void saveCurrentImages();

	public:

	// Constructor, with optional height and width as input
	DetectionSummary(int h = 640, int w = 640);

	// Destructor, saves last image
	~DetectionSummary();

	// Set output prefix
	inline void setOutputPrefix(std::string p) { prefix = p; }

	// Add a detection to the current output images
	// - object: next object to write
	// - mask: object's binary mask
	void addDetection(const cv::Mat& object, const cv::Mat& mask);
};

#endif
