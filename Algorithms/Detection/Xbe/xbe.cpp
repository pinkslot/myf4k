#include "xbe.h"
#include <time_utils.h>
#include <log.h>
#include <highgui.h>
#include <opencv_utils.h>

using namespace cv;

namespace alg
{

	Xbe::Xbe() :
		feature_mask(-1)
	{
		// Setup parameters
		// The feature_mask parameters allows to specify which color spaces to use. Just add the constant defined in the .h file to build the feature set.
		parameters.add<long int>("feature_mask", GS + 0);
		parameters.add<int>("init_window", 3);
		parameters.add<int>("history", 20);
		parameters.add<int>("threshold", 2);
		parameters.add<float>("squared_radius", 20*20);
		parameters.add<int>("update_sampling", 16);
		parameters.add<int>("propagation_window", 3);
		//parameters.add<bool>("debug", false);
	}

	// Convert image to Xvalue matrix
	Matrix<Xbe::Xvalue> Xbe::convertImage(const Mat& frame)
	{
		TimeInterval t1;
		t1.tic();
		// Define color-space matrices
		Mat frame_rgb = frame.clone(), frame_hsv, frame_lab, frame_yiq, frame_gs;//, frame_hvc;
		// Check which conversions are needed
		if(checkFeature(HSV_H) || checkFeature(HSV_S) || checkFeature(HSV_V))
		{
			// HSV conversion
			cvtColor(frame, frame_hsv, CV_BGR2HSV);
			// Split channels
			vector<Mat> channels;
			split(frame_hsv, channels);
			// Rescale hue channels
			channels[0] = channels[0]*1.41666666;
			// Re-merge channels
			merge(channels, frame_hsv);
		}
		if(checkFeature(Lab_L) || checkFeature(Lab_a) || checkFeature(Lab_b))
		{
			// Lab conversion
			cvtColor(frame, frame_lab, CV_BGR2Lab);
		}
		if(checkFeature(YIQ_Y) || checkFeature(YIQ_I) || checkFeature(YIQ_Q))
		{
			// YIQ conversion
			frame_yiq = cvtColorYIQ(frame);
		}
		if(checkFeature(GS))
		{
			// Gray-scale conversion
			cvtColor(frame, frame_gs, CV_BGR2GRAY);
		}
		/*if(checkFeature(Munsell_H) || checkFeature(Munsell_V) || checkFeature(Munsell_C))
		{
			// Munsell conversion

		}*/
		// Convert input image to matrix of Xvalue
		Matrix<Xvalue> output(frame.rows, frame.cols);
		for(int i=0; i<frame.rows; i++)
		{
			for(int j=0; j<frame.cols; j++)
			{
				// Get reference to pixel
				Xvalue& x_value = output(i,j);
				// Check each feature
				if(checkFeature(RGB_R))	x_value.push_back(frame.at<Vec3b>(i,j)[2]);
				if(checkFeature(RGB_G))	x_value.push_back(frame.at<Vec3b>(i,j)[1]);
				if(checkFeature(RGB_B))	x_value.push_back(frame.at<Vec3b>(i,j)[0]);
				if(checkFeature(HSV_H))	x_value.push_back(frame_hsv.at<Vec3b>(i,j)[0]);
				if(checkFeature(HSV_S))	x_value.push_back(frame_hsv.at<Vec3b>(i,j)[1]);
				if(checkFeature(HSV_V))	x_value.push_back(frame_hsv.at<Vec3b>(i,j)[2]);
				if(checkFeature(Lab_L))	x_value.push_back(frame_lab.at<Vec3b>(i,j)[0]);
				if(checkFeature(Lab_a))	x_value.push_back(frame_lab.at<Vec3b>(i,j)[1]);
				if(checkFeature(Lab_b))	x_value.push_back(frame_lab.at<Vec3b>(i,j)[2]);
				if(checkFeature(YIQ_Y))	x_value.push_back(frame_yiq.at<Vec3b>(i,j)[0]);
				if(checkFeature(YIQ_I))	x_value.push_back(frame_yiq.at<Vec3b>(i,j)[1]);
				if(checkFeature(YIQ_Q))	x_value.push_back(frame_yiq.at<Vec3b>(i,j)[2]);
				// Munsell stuff
				// ...
				if(checkFeature(GS))	x_value.push_back(frame_gs.at<uchar>(i,j));
			}
		}
		Log::d() << "Image conversion took " << t1.toc() << " s." << endl;
		// Return result
		return output;
	}
		
	void Xbe::initModel(const Matrix<Xvalue>& x_frame)
	{
		// Read parameters
		int init_window = parameters.get<int>("init_window");
		int history = parameters.get<int>("history");
		Log::d() << "Initializing Xbe model." << endl;
		// Read image size
		int rows = x_frame.rows();
		int cols = x_frame.cols();
		// Initialize each pixel
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols; j++)
			{
				// Get background element
				Xpixel& pixel = background(i,j);
				// Compute minimum distance from the borders
				int min_dist = i;
				if(j < min_dist)		min_dist = j;
				if((rows-i-1) < min_dist)	min_dist = rows-i-1;
				if((cols-j-1) < min_dist)	min_dist = cols-j-1;
				if(init_window < min_dist)	min_dist = init_window;
				// Get window coordinates
				int min_i = i - min_dist/2;//	if(min_i < 0)			min_i = 0;
				int max_i = i + min_dist/2;//	if(max_i >= x_frame.rows())	max_i = x_frame.rows() - 1;
				int min_j = j - min_dist/2;//	if(min_j < 0)			min_j = 0;
				int max_j = j + min_dist/2;//	if(max_j >= x_frame.cols())	max_j = x_frame.cols() - 1;
				// Select random samples from candidates
				while(true)
				{
					for(int k=min_i; k<=max_i; k++)
					{
						for(int l=min_j; l<=max_j; l++)
						{
							// Check history full
							if(((int)pixel.size()) == history)
							{
								goto HISTORY_FULL; // out of all loops
							}
							// Add to history
							pixel.push_back(x_frame(k,l));
						}
					}
				}
				// History filled
				HISTORY_FULL:
				;
			}
		}
		Log::d() << "Xbe model initialized." << endl;
	}
		
	// Update model from frame and foreground mask
	void Xbe::updateModel(const Matrix<Xvalue>& x_frame, const cv::Mat& mask)
	{
		// Copy parameters
		int update_sampling = parameters.get<int>("update_sampling");
		int history = parameters.get<int>("history");
		int propagation_window = parameters.get<int>("propagation_window");
		// Read image size
		int rows = x_frame.rows();
		int cols = x_frame.cols();
		// Compute random matrices
		Mat rand_mask(x_frame.rows(), x_frame.cols(), CV_8UC1);
		Mat rand_propagation(x_frame.rows(), x_frame.cols(), CV_16UC2);
		theRNG().fill(rand_mask, RNG::UNIFORM, 0, update_sampling);
		theRNG().fill(rand_propagation, RNG::UNIFORM, -propagation_window/2, propagation_window/2+1);
		// Go through all pixels, check only the ones having rand_mask=0 and mask=0
		for(int i=0; i<rows; i++)
		{
			for(int j=0; j<cols; j++)
			{
				// Check pixel
				if(rand_mask.at<uchar>(i,j) == 0 && mask.at<uchar>(i,j) == 0)
				{
					// Select pixel to substitute
					int replace = theRNG().uniform(0, history);
					// Replace it in the background model
					background(i,j)[replace] = x_frame(i,j);
					// Read which neighbour to propagate this value to, from the rand_propagation matrices
					Vec2s n_delta = rand_propagation.at<Vec2s>(i,j);
					int n_i = i + n_delta[0];
					int n_j = j + n_delta[1];
					// Check boundaries
					if(n_i < 0)		n_i = 0;
					else if(n_i >= rows)	n_i = rows-1;
					if(n_j < 0)		n_j = 0;
					else if(n_j >= cols)	n_j = cols-1;
					// Propagate value to neighbour
					background(n_i,n_j)[replace] = x_frame(i,j);
				}
			}
		}
	}

	Mat Xbe::nextFrame(Context& C)
	{
		// Copy parameters
		float squared_radius = parameters.get<float>("squared_radius");
		int threshold = parameters.get<int>("threshold");
		int history = parameters.get<int>("history");
		//bool debug = parameters.get<bool>("debug");
		// Set cached feature mask
		if(feature_mask == -1)	feature_mask = parameters.get<long int>("feature_mask");
		// Copy frame to local matrix
		Mat frame = C.preproc_output.clone();
		// Convert to Xvalues
		Matrix<Xvalue> x_frame = convertImage(frame);
		// Check initialization
		if(background.getData() == NULL)
		{
			// Allocate
			background.setSize(frame.rows, frame.cols);
			// Initialize model
			initModel(x_frame);
		}
		// Initialize background mask
		Mat mask = 255*Mat::ones(frame.rows, frame.cols, CV_8UC1);
		// Check each pixel
		for(int i=0; i<frame.rows; i++)
		{
			for(int j=0; j<frame.cols; j++)
			{
				// Get background pixel
				Xpixel& x_pixel = background(i,j);
				// Get current value
				Xvalue& x_value = x_frame(i,j);
				// Initialize number of history pixel within the radius from the current value
				int bg_matches = 0;
				// Go throuh all history values
				for(Xpixel::iterator v_it = x_pixel.begin(); v_it != x_pixel.end(); v_it++)
				{
					// Get history value
					Xvalue& h_value = *v_it;
					// Compute squared distance from current value
					float sqr_dist = 0.0f;
					for(unsigned int k=0; k<h_value.size(); k++)
					{
						float diff = h_value[k]-x_value[k];
						sqr_dist += diff*diff; 
					}
					//cout << i << " " << j << " " << sqr_dist << endl;
					// Check
					if(sqr_dist < squared_radius)
					{
						// Increase match count
						bg_matches++;
						// Check match count
						if(bg_matches >= threshold)
						{
							// Set pixel as white in the mask
							mask.at<uchar>(i,j) = 0;
							// Go to next pixel
							break;
						}
					}

				}
			}
		}
		// Update background model
		updateModel(x_frame, mask);
		// Debug, show background model
		//if(debug)
		//{
			//imshow("mask", mask);
			/*int num_features = background(0,0)[0].size();
			vector<Mat> bgs;
			for(int i=0; i<num_features; i++)
			{
				bgs.push_back(Mat::zeros(x_frame.rows(), x_frame.cols(), CV_32FC1));
			}
			for(int i=0; i<x_frame.rows(); i++)
			{
				for(int j=0; j<x_frame.cols(); j++)
				{
					for(int h=0; h<history; h++)
					{
						for(int f=0; f<num_features; f++)
						{
							//cout << i << " " << j << ": " << background(i,j)[h][f] << endl;
							bgs[f].at<float>(i,j) += background(i,j)[h][f];
						}
					}
				}
			}
			for(int i=0; i<num_features; i++)
			{
				bgs[i] = bgs[i] / history;
				Mat tmp;
				bgs[i].convertTo(tmp, CV_8UC1);
				stringstream ss;
				ss << "background_" << i;
				imshow(ss.str(), tmp);
			}
			// Debug, show current image
			vector<Mat> cs;
			for(int i=0; i<num_features; i++)
			{
				cs.push_back(Mat::zeros(x_frame.rows(), x_frame.cols(), CV_32FC1));
			}
			for(int i=0; i<x_frame.rows(); i++)
			{
				for(int j=0; j<x_frame.cols(); j++)
				{
					for(int f=0; f<num_features; f++)
					{
						cs[f].at<float>(i,j) = x_frame(i,j)[f];

					}
				}
			}
			for(int i=0; i<num_features; i++)
			{
				Mat tmp;
				cs[i].convertTo(tmp, CV_8UC1);
				stringstream ss;
				ss << "current_" << i;
				imshow(ss.str(), tmp);
			}*/
		//}
		// Write result
		C.motion_output = mask;
		return C.motion_output;
	}	

	Xbe::~Xbe()
	{
	}

}
