#include "vibe2.h"
#include <highgui.h>

using namespace cv;

namespace alg
{

	ViBE2::ViBE2(void)
	{
		// Setup parameters
		parameters.add<int>("radius", 20);
		parameters.add<int>("threshold", 2);
		parameters.add<int>("update_rate", 8);
		parameters.add<int>("history", 20);
		parameters.add<int>("learning", 50);
		// Set init flag to true
		init = true;
		// Set internal variables to NULL; will be initialized when the first frame arrives
		model = NULL;
		image_data = NULL;
		segmentation_map = NULL;
	}

	ViBE2::~ViBE2(void)
	{
		// Free memory
		if(model != NULL)		libvibeModelFree(model);
		if(image_data != NULL)		delete [] image_data;
		if(segmentation_map != NULL)	delete [] segmentation_map;
	}

	Mat ViBE2::nextFrame(Context& context)
	{
		// Convert to Mat
		Mat frame = context.preproc_output;
		// Convert from BGR to RGB
		//Mat rgb_frame;
		//cvtColor(frame, rgb_frame, CV_BGR2RGB);
		// Read paramters
		int learning = parameters.get<int>("learning");
		// Check initialization
		if(init)
		{
			// Initialize learning buffer
			learning_buffer = new Mat[learning];
			// Reset init
			init = false;
		}
		// Check frame number
		if(context.frame_num < learning)
		{
			// Save to buffer
			learning_buffer[context.frame_num] = frame.clone();
			// Check if we have to return
			if(context.frame_num < (learning-1))
			{
				Log::debug() << "Adding to background learning buffer frame " << (context.frame_num+1) << "/" << learning << endl;
				// Return black frame
				context.motion_output = Mat::zeros(frame.rows, frame.cols, CV_8UC1);
				return context.motion_output;
			}
			else // if frame_num == learning-1, i.e. we have all necessary frames
			{
				Log::debug() << "Adding to background learning buffer frame " << (context.frame_num+1) << "/" << learning << ", can initialize model" << endl;
				// Return black frame
				// Compute median image
				Mat output(frame.rows, frame.cols, CV_8UC3);
				// Check each pixel
				for(int i=0; i<frame.rows; i++)
				{
					for(int j=0; j<frame.cols; j++)
					{
						// Initialize lists
						vector<uchar> r_list, g_list, b_list;
						// Read values
						for(int t=0; t<learning; t++)
						{
							// Get pixel
							const Vec3b& pixel = learning_buffer[t].at<Vec3b>(i,j);
							// Add to list
							b_list.push_back(pixel[0]);
							g_list.push_back(pixel[1]);
							r_list.push_back(pixel[2]);
						}
						// Sort lists
						sort(b_list.begin(), b_list.end());
						sort(g_list.begin(), g_list.end());
						sort(r_list.begin(), r_list.end());
						// Get values
						uchar median_b = b_list[learning/2];
						uchar median_g = g_list[learning/2];
						uchar median_r = r_list[learning/2];
						// Write value
						output.at<Vec3b>(i,j) = Vec3b(median_b, median_g, median_r);
					}
				}
				// Initialize model
				model = libvibeModelNew();
				libvibeModelSetMatchingThreshold(model, parameters.get<int>("radius"));
				libvibeModelSetMatchingNumber(model, parameters.get<int>("threshold"));
				libvibeModelSetNumberOfSamples(model, parameters.get<int>("history"));
				libvibeModelSetUpdateFactor(model, parameters.get<int>("update_rate"));
				// Read image dimensions
				width = frame.cols;
				height = frame.rows;
				stride = frame.cols*frame.elemSize();
				// Allocate memory
				image_data = new uint8_t[stride*height];
				segmentation_map = new uint8_t[stride*height];
				// Read the data into image_data
				int current_byte = 0;
				for(int i=0; i<height; i++)
				{
					for(int j=0; j<width; j++)
					{
						Vec3b pixel = output.at<Vec3b>(i,j);
						image_data[current_byte++] = pixel[2];
						image_data[current_byte++] = pixel[1];
						image_data[current_byte++] = pixel[0];
					}
				}
				// Initialize model with first image
				libvibeModelAllocInit_8u_C3R(model, image_data, width, height, stride);
			}
		}
		else
		{
			// Read the data into image_data
			int current_byte = 0;
			for(int i=0; i<height; i++)
			{
				for(int j=0; j<width; j++)
				{
					Vec3b pixel = frame.at<Vec3b>(i,j);
					image_data[current_byte++] = pixel[2];
					image_data[current_byte++] = pixel[1];
					image_data[current_byte++] = pixel[0];
				}
			}
			// Update model
			libvibeModelUpdate_8u_C3R(model, image_data, segmentation_map);
		}
		// Convert segmentation map to image
		Mat seg_image(height, width, CV_8UC1, segmentation_map);
		// Remove side borders
		for(int i=0; i<height; i++)
		{
			for(int j=0; j<5; j++)
			{
				seg_image.at<uchar>(i,j) = 0;
			}
			for(int j=width-5; j<width; j++)
			{
				seg_image.at<uchar>(i,j) = 0;
			}
		}
		// Remove isolated white pixels
		Mat median_out;
		medianBlur(seg_image, median_out, 3);
		// Show image
		//imshow("segm", median_out);
		//waitKey(0);
		// Set to context and return
		context.motion_output = median_out;
		return median_out;
	}

}
