// Contrast-stretching based image preprocessing

#include "contrast_stretching.h"
#include <iostream>
#include <highgui.h>

using namespace cv;
using namespace std;

// Constructor; initialize parameters
alg::ContrastStretching::ContrastStretching()
{
	parameters.add<float>("trim_threshold_b", 0.0025f);
	parameters.add<float>("trim_threshold_g", 0.0025f);
	parameters.add<float>("trim_threshold_r", 0.0004f);
}

// Apply contrast stretching to current frame
// (Assuming BGR color)
Mat alg::ContrastStretching::nextFrame(const Mat& frame)
{
	// Split the image into the three channels
	Mat* channels = new Mat[frame.channels()];
	split(frame, channels);
	// Compute histograms
	Histogram hist_b(channels[0]);
	Histogram hist_g(channels[1]);
	Histogram hist_r(channels[2]);
	// Trim histograms at the extremities and get boundary bins
	int min[3], max[3];
	float threshold_b = parameters.get<float>("trim_threshold_b"); //0.0025f;
	float threshold_g = parameters.get<float>("trim_threshold_g"); //0.0025f;
	float threshold_r = parameters.get<float>("trim_threshold_r"); //0.0004f;
	hist_b.trim(threshold_b, &(min[0]), &(max[0]));
	hist_g.trim(threshold_g, &(min[1]), &(max[1]));
	hist_r.trim(threshold_r, &(min[2]), &(max[2]));
	// Compute min and max value
	/*double min = 255, max = 0;
	for(int c=0; c<frame.channels(); c++)
	{
		// Get reference to channel image
		Mat& img = channels[c];
		// Get minimum and maximum
		double local_min, local_max;
		minMaxLoc(img, &local_min, &local_max);
		// Compare to current min and max
		if(local_min < min)	min = local_min;
		if(local_max > max)	max = local_max;
	}
	cout << min << " " << max << endl;*/
	// Process each channel
	double low_out = 0;
	double high_out = 255;
	for(int c=0; c<frame.channels(); c++)
	{
		//cout << "channel " << min[c] << " " << max[c] << endl;
		for(int i=0; i<channels[c].rows; i++)
		{
			for(int j=0; j<channels[c].cols; j++)
			{
				// Get pixel value
				uchar& pixel = channels[c].at<uchar>(i,j);
				// Clip if necessary
				double value = pixel;
				if(value < min[c])	value = min[c];
				if(value > max[c])	value = max[c];
				// Update
				pixel = saturate_cast<uchar>(low_out + (value - min[c])*(high_out - low_out)/(max[c] - min[c]));
			}
		}
	}
	// Merge channels
	Mat stretched_frame;
	merge(channels, frame.channels(), stretched_frame);
	// Convert to HSV
	Mat hsv;
	cvtColor(stretched_frame, hsv, CV_BGR2HSV);
	// Split planes
	split(hsv, channels);
	// Modify S and V
	channels[1] = channels[1] + 25;//38;
	channels[2] = channels[2] + 18;//23;
	// Re-join channels
	merge(channels, 3, hsv);
	// Reconvert to BGR
	Mat bgr;
	cvtColor(hsv, bgr, CV_HSV2BGR);
	// Free matrices
	delete [] channels;
	// Return result
	return bgr;
	//imshow("r", channels[2]);
	//imshow("g", channels[1]);
	//imshow("b", channels[0]);
	// Sets thresholds
	/*float threshold_b = 0.0025f;
	float threshold_g = 0.0025f;
	float threshold_r = 0.0004f;
	// Compute histograms
	Histogram hist_b(channels[0]);
	Histogram hist_g(channels[1]);
	Histogram hist_r(channels[2]);
	//cout << "hist_b = " << hist_b << endl;
	//cout << "hist_g = " << hist_g << endl;
	//cout << "hist_r = " << hist_r << endl;
	// Trim histograms at the extremities and get boundary bins
	int min_b = 0, min_g = 0, min_r = 0, max_b = 255, max_g = 255, max_r = 255;
	hist_b.trim(threshold_b, &min_b, &max_b);
	hist_g.trim(threshold_g, &min_g, &max_g);
	hist_r.trim(threshold_r, &min_r, &max_r);
	//cout << min_b << " " << max_b << endl;
	//cout << min_g << " " << max_g << endl;
	//cout << min_r << " " << max_r << endl;
	// Equalize histograms
	int mapping_b[256];
	int mapping_g[256];
	int mapping_r[256];
	hist_b.equalize(mapping_b);
	hist_g.equalize(mapping_g);
	hist_r.equalize(mapping_r);
	//Mat r_cveq, g_cveq, b_cveq;
	//equalizeHist(channels[2], r_cveq);
	//equalizeHist(channels[1], g_cveq);
	//equalizeHist(channels[0], b_cveq);
	//imshow("r_cveq", r_cveq);
	//imshow("g_cveq", g_cveq);
	//imshow("b_cveq", b_cveq);
	// Update images
	for(int i=0; i<rows; i++)
	{
		for(int j=0; j<cols; j++)
		{
			//uchar& b = channels[0].at<uchar>(i,j);
			//uchar& g = channels[1].at<uchar>(i,j);
			//uchar& r = channels[2].at<uchar>(i,j);
			channels[0].at<uchar>(i,j) = mapping_b[channels[0].at<uchar>(i,j)];
			channels[1].at<uchar>(i,j) = mapping_g[channels[1].at<uchar>(i,j)];
			channels[2].at<uchar>(i,j) = mapping_r[channels[2].at<uchar>(i,j)];
		}
	}
	//imshow("r_eq", channels[2]);
	//imshow("g_eq", channels[1]);
	//imshow("b_eq", channels[0]);
	// Re-join channels
	Mat eq_frame;
	merge(channels, 3, eq_frame);
	//Mat cveq_channels[3];
	//Mat cveq_frame;
	//cveq_channels[0] = b_cveq;
	//cveq_channels[1] = g_cveq;
	//cveq_channels[2] = r_cveq;
	//merge(cveq_channels, 3, cveq_frame);
	//7imshow("eq_frame", eq_frame);
	//imshow("cveq_frame", cveq_frame);
	// Convert to HSV
	Mat hsv;
	cvtColor(eq_frame, hsv, CV_BGR2HSV);
	// Split planes
	split(hsv, channels);
	// Modify S and V
	//channels[2] = channels[2] + 70;
	// Re-join channels
	merge(channels, 3, hsv);
	// Reconvert to BGR
	Mat bgr;
	cvtColor(hsv, bgr, CV_HSV2BGR);
	// Free matrices
	delete [] channels;
	// Return result
	return bgr;*/

}
