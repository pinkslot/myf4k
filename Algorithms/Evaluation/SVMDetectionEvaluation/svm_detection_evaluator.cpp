#include "svm_detection_evaluator.h"
#include <cvblob.h>
#include <lk_motion_vector.h>
#include <motion_vector_utils.h>
#include <time_utils.h>
#include <math_utils.h>
#include <log.h>
#include <xhistogram.h>
// Debug includes
#include <iostream>
#include <highgui.h>

using namespace cv;
using namespace std;
using namespace alg;

// Constructor
SVMDetectionEvaluator::SVMDetectionEvaluator(int frame_buffer_size) :
	Evaluator(frame_buffer_size)
{
	// Initialize SVM
	svm.loadBuiltIn();
	// Resize thread pool
	thread_pool.size_controller().resize(20);
	// Parameters for boundary complexity score
	parameters.add<int>("gs_b_s", 5);
	parameters.add<int>("gs_b_k", 10);
	parameters.add<int>("gs_b_sn", 5);
	// Parameters for color contrast
	parameters.add<int>("cc_theta", 3);
	parameters.add<int>("cc_color_space", CV_BGR2HSV);
	parameters.add<int>("cc_channel", 0); // 0: H
	parameters.add<float>("cc_bin_step", 0.05);
	// Parameters for Canny filters
	parameters.add<int>("canny_t", 25);
	// Parameters for superpixels straddling
	parameters.add<float>("sp_sigma", 0.5);
	parameters.add<int>("sp_k", 125);
	parameters.add<int>("sp_min", 100);
	// Parameters for motion boundary
	parameters.add<int>("win_mb", 10);
	parameters.add<int>("level_mb", 0);
	parameters.add<int>("iter_mb", 5);
	parameters.add<int>("channel_mb", 0); // 0: modulus
	parameters.add<int>("padding_mb", 15);
	parameters.add<int>("theta_in_mb", 3);
	parameters.add<int>("theta_out_mb", 7);
	parameters.add<float>("bin_step_mb", 0.5);
	// Parameters for motion internal
	parameters.add<int>("win_mi", 20);
	parameters.add<int>("level_mi", 0);
	parameters.add<int>("iter_mi", 5);
	parameters.add<int>("channel_mi", 0); // 0: modulus
	parameters.add<string>("stat_mi", "mean");
	// Parameters for color internal
	parameters.add<int>("ci_color_space", CV_BGR2HSV);
	parameters.add<int>("ci_channel", 0); // 0: H
	parameters.add<string>("ci_stat", "mean");
}

// Perform score computation of detection in the current frame
void SVMDetectionEvaluator::addFrame(const vector<const TrackedObject*>& tracked_objects, const Mat& frame, int frame_number, const Mat* next_frame)
{
	// Add current frame to buffer
	frame_buffer->add(frame.clone());
	// Process each tracked object
	boost::thread_group threads;
	for(vector<const TrackedObject*>::const_iterator it = tracked_objects.begin(); it != tracked_objects.end(); it++)
	{
		// Get reference
		const TrackedObject& tracked_object = **it;
		// Check that the object appears in this frame
		if(tracked_object.lastAppearance() != frame_number)
		{
			// Skip object
			continue;
		}
		// If we don't have a ScoreHistory for this object, create it
		objects_mutex.lock();
		if(objects.find(tracked_object.id) == objects.end())
		{
			ScoreHistory* history = new ScoreHistory();
			// Add history to object list
			objects[tracked_object.id] = history;
		}
		ScoreHistory* history = objects[tracked_object.id];
		// Add new score to history
		SVMDetectionScore* score = new SVMDetectionScore(&svm);
		history->addScore(score, frame_number);
		objects_mutex.unlock();
		// Get object's blob
		const cvb::CvBlob& blob = tracked_object.currentRegion();
		// Check if the detection certainty for this object has already been computed
		if(blob.detection_certainty >= 0)
		{
			log_mutex.lock();
			Log::info() << "Detection certainty already computed: " << blob.detection_certainty << endl;
			log_mutex.unlock();
			score->cached_score = blob.detection_certainty;
			continue;
		}
		// Copy blob
		cvb::CvBlob* copy = new cvb::CvBlob();
		cvCloneBlob(blob, *copy);
		// Run thread
		boost::thread* thread = new boost::thread(boost::bind(&SVMDetectionEvaluator::computeScoreThread, this, _1, _2, _3, _4), boost::ref(*copy), boost::ref(frame), boost::ref(*score), next_frame);
		log_mutex.lock();
		Log::d() << "Create new detection evaluation thread." << endl;
		log_mutex.unlock();
		threads.add_thread(thread);
	}	
	// Wait for all threads
	threads.join_all();
}
		
// Compute the boundary complexity score
void SVMDetectionEvaluator::computeBoundaryComplexityScore(const cvb::CvBlob& blob, int rows, int cols, float& result)
{
	result = -1;
	try
	{
		// Read parameters
		int b_s = parameters.get<int>("gs_b_s");
		int b_k = parameters.get<int>("gs_b_k");
		int b_sn = parameters.get<int>("gs_b_sn");
		// Convert blob to mask
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, cols, rows);
		// Convert blob to point vector
		vector<Point> contour = blobToPointVector(blob);
		// Compute boundary complexity
		result = boundaryComplexity(mask, contour, b_s, b_k, b_sn);
	}
	catch(MyException& e)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing boundary complexity score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing boundary complexity score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}
		
// Compute the superpixels straddling score
void SVMDetectionEvaluator::computeSuperpixelsStraddlingScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result)
{
	try
	{
		// Copy parameters
		float sigma = parameters.get<float>("sp_sigma");
		int k = parameters.get<float>("sp_k");
		int min_size = parameters.get<float>("sp_min");
		// Rebuild mask
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, frame.cols, frame.rows);
		// Crop frame and mask
		int min_x = blob.minx;
		int min_y = blob.miny;
		int max_x = blob.maxx;
		int max_y = blob.maxy;
		min_x -= 20; if(min_x < 0)		min_x = 0;
		min_y -= 20; if(min_y < 0)		min_y = 0;
		max_x += 20; if(max_x >= frame.cols)	max_x = frame.cols-1;
		max_y += 20; if(max_y >= frame.rows)	max_y = frame.rows-1;
		Mat sub_frame = frame(Range(min_y, max_y+1), Range(min_x, max_x+1)); // End value in range is excluded
		Mat sub_mask = mask(Range(min_y, max_y+1), Range(min_x, max_x+1)); // End value in range is excluded
		// Perform segmentation
		Mat sub_seg;
		int num_sp;
		segmentSuperpixels(sub_frame, sub_seg, num_sp, k, min_size);
		// Compute result
		result = superpixelsStraddlingScore(sub_seg, sub_mask);
	}
	catch(MyException& e)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing superpixel straddling score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing superpixel straddling score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}
		
// Compute the edge density score for an object
/*void SVMDetectionEvaluator::computeEdgeDensityScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result)
{
	result = -1;
	try
	{
		// Read parameters
		float t1 = parameters.get<int>("canny_t");
		float t2 = 0.4*t1;
		// Convert to grayscale
		Mat gray;
		cvtColor(frame, gray, CV_BGR2GRAY);
		// Apply Canny filters
		Mat canny_1;
		Canny(gray, canny_1, t1, t2);
		// Select only blob's region
		Mat mask = blobToMat(blob, CV_POSITION_TOP_LEFT, true);
		Mat mask_no_contour = blobToMat(blob, CV_POSITION_TOP_LEFT, false);
		Mat canny_1_mask = canny_1(Rect(blob.x, blob.y, blob.width(), blob.height()));
		//imshow("canny 1", canny_1);
		//imshow("canny 2", canny_2);
		// Compute ratio of white pixels
		float score_1 = 0.0f;
		for(int i=0; i<mask.rows; ++i)
		{
			for(int j=0; j<mask.cols; ++j)
			{
				if(mask.at<uchar>(i,j) > 0 && mask_no_contour.at<uchar>(i,j) == 0 && canny_1_mask.at<uchar>(i,j) > 0)
				{
					score_1++;
				}
			}
		}
		result = score_1/blob.area;
	}
	catch(MyException& e)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing edge density score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing edge density score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}*/

// Compute the color internal score for an object
void SVMDetectionEvaluator::computeColorInternalScore(const Mat& frame, const cvb::CvBlob& blob, float& result)
{
	// Initialize result to invalid value
	result = -1;
	try
        {
		// Copy parameters
		int color_space = parameters.get<int>("ci_color_space");
		int channel = parameters.get<int>("ci_channel");
                // Rebuild blob mask
                Mat mask = drawBlob(blob, true);
                // Convert to destination color space
                Mat conv;
                cvtColor(frame, conv, color_space);
		conv.convertTo(conv, CV_32FC3, 1.0/180.0);
                // Initialize lists
                vector<float> values;
                // Go through all mask pixels
                for(unsigned int x=blob.x; x<=blob.maxx; x++)
                {
                        for(unsigned int y=blob.y; y<=blob.maxy; y++)
                        {
                                // Check mask
                                if(mask.at<uchar>(y,x) > 0)
                                {
                                        // Read motion vector in this point
                                        Vec3f pixel = conv.at<Vec3f>(y,x);
                                        // Add to lists
                                        values.push_back(pixel[channel]);
                                }
                        }
                }
                // Compute means and std devs
                float mean_value, std_value;
                MathUtils::computeMeanAndStdDev<float>(values, mean_value, std_value);
                // Set result
                result = mean_value;
        }
	catch(MyException& e)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color internal score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color internal score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}

// Compute the motion internal score for an object
void SVMDetectionEvaluator::computeMotionInternalScore(const Mat& frame, const Mat* next_frame, const cvb::CvBlob& blob, float& result)
{
	// Initialize result to invalid value
	result = -1;
	try
        {
		// Copy parameters
		int mv_win = parameters.get<int>("win_mi");
		int mv_max_level = parameters.get<int>("level_mi");
		int mv_max_iter = parameters.get<int>("iter_mi");
		int channel = parameters.get<int>("channel_mi");
		string stat = parameters.get<string>("stat_mi");
		// Initialize motion vector
		LKMotionVector mv;
		mv.window_size = mv_win;
		mv.max_level = mv_max_level;
		Mat motion_vector;
		// Compute rect
		Rect curr_rect(blob.x, blob.y, blob.width(), blob.height());
		// Compute motion vector
		mv.addFrame(frame);
		((MotionVector*) &mv)->addFrame(*next_frame, curr_rect, motion_vector);
                // Rebuild blob mask
                Mat mask = drawBlob(blob, true);
                // Initialize value list
                vector<float> values;
                // Go through all mask pixels
                for(unsigned int x=blob.x; x<blob.maxx; x++)
                {
                        for(unsigned int y=blob.y; y<blob.maxy; y++)
                        {
                                // Check mask
                                if(mask.at<uchar>(y,x) > 0)
                                {
                                        // Read motion vector in this point
                                        Vec4d mv_point = motion_vector.at<Vec4d>(y,x);
                                        // Add to lists
                                        values.push_back(mv_point[channel]);
                                }
                        }
                }
                // Compute mean
                float mean_value, std_value;
                MathUtils::computeMeanAndStdDev<float>(values, mean_value, std_value);
                // Set result
                result = mean_value;
        }
	catch(MyException& e)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion internal score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion internal score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}
	
// Compute the motion_boundary_score value for an object
void SVMDetectionEvaluator::computeMotionBoundaryScore(const Mat& frame, const Mat* next_frame, const cvb::CvBlob& blob, float& result)
{
	// Initialize result
	result = -1;
	// Check next frame
	if(next_frame == NULL)
	{
		Log::i() << "Next frame null, returning -1" << endl;
		return;
	}
	// Initialize blobs
	cvb::CvBlobs blobs_in;
	cvb::CvBlobs blobs_out;
	try
	{
		// Copy parameters
		int mv_win = parameters.get<int>("win_mb");
		int mv_max_level = parameters.get<int>("level_mb");
		int mv_max_iter = parameters.get<int>("iter_mb");
		int theta_in = parameters.get<int>("theta_in_mb");
		int theta_out = parameters.get<int>("theta_out_mb");
		int channel = parameters.get<int>("channel_mb");
		float bin_step = parameters.get<float>("bin_step_mb");
		int padding = parameters.get<int>("padding_mb");
		// Initialize motion vector
		LKMotionVector mv;
		mv.window_size = mv_win;
		mv.max_level = mv_max_level;
		Mat motion_vector;
		// Add padding
		Rect curr_rect(blob.x, blob.y, blob.width(), blob.height());
		curr_rect -= Point(padding, padding);
		curr_rect += Size(2*padding, 2*padding);
		// Check limits
		if(curr_rect.x < 0)					curr_rect.x = 0;
		if(curr_rect.y < 0)					curr_rect.y = 0;
		if(curr_rect.x + curr_rect.width - 1 >= frame.cols)	curr_rect.width = frame.cols - curr_rect.x;
		if(curr_rect.y + curr_rect.height - 1 >= frame.rows)	curr_rect.height = frame.rows - curr_rect.y;
		// Compute motion vector
		mv.addFrame(frame);
		((MotionVector*) &mv)->addFrame(*next_frame, curr_rect, motion_vector);
		// Convert blob to binary mask
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, motion_vector.cols, motion_vector.rows);
		// Dilate and erode mask by theta pixels
		Mat mask_in, mask_out;
		dilate(mask, mask_out, Mat(), Point(-1,-1), theta_out);
		erode(mask, mask_in, Mat(), Point(-1,-1), theta_in);
		blobs_in = computeBlobs(mask_in);
		blobs_out = computeBlobs(mask_out);
		// Check number of blobs
		if(blobs_in.size() == 0 || blobs_out.size() == 0)
		{
			throw MyException("Error computing inner and/or outer blobs for motion boundary.");
		}
		// Get blobs references
		cvb::CvBlob& blob_in = *(blobs_in.begin()->second);
		cvb::CvBlob& blob_out = *(blobs_out.begin()->second);
		// Initialize value vectors
		vector<float> values_in, values_out;
		// Initialize dx and dy ranges
		float min_value = INT_MAX, max_value = INT_MIN;
		// Select pixels
		Point current_in = blob_in.contour.startingPoint;
		Vec4d mv_in = motion_vector.at<Vec4d>(current_in.y, current_in.x);
		if(mv_in[channel] < min_value)	min_value = mv_in[channel];
		if(mv_in[channel] > max_value)	max_value = mv_in[channel];
		values_in.push_back(mv_in[channel]);
		const cvb::CvChainCodes& chain_code_in = blob_in.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_in.begin(); it != chain_code_in.end(); it++)
		{
			current_in.x += cvb::cvChainCodeMoves[*it][0];
			current_in.y += cvb::cvChainCodeMoves[*it][1];
			Vec4d mv_in = motion_vector.at<Vec4d>(current_in.y, current_in.x);
			if(mv_in[channel] < min_value)	min_value = mv_in[channel];
			if(mv_in[channel] > max_value)	max_value = mv_in[channel];
			values_in.push_back(mv_in[channel]);
		}
		// Select outer pixels
		Point current_out = blob_out.contour.startingPoint;
		Vec4d mv_out = motion_vector.at<Vec4d>(current_out.y, current_out.x);
		if(mv_out[channel] < min_value)	min_value = mv_out[channel];
		if(mv_out[channel] > max_value)	max_value = mv_out[channel];
		values_out.push_back(mv_out[channel]);
		const cvb::CvChainCodes& chain_code_out = blob_out.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_out.begin(); it != chain_code_out.end(); it++)
		{
			current_out.x += cvb::cvChainCodeMoves[*it][0];
			current_out.y += cvb::cvChainCodeMoves[*it][1];
			Vec4d mv_out = motion_vector.at<Vec4d>(current_out.y, current_out.x);
			if(mv_out[channel] < min_value)	min_value = mv_out[channel];
			if(mv_out[channel] > max_value)	max_value = mv_out[channel];
			values_out.push_back(mv_out[channel]);
		}
		// Check list sizes
		if(values_in.size() == 0 || values_out.size() == 0)
		{
			throw MyException("Empty histograms for motion boundary.");
		}
		// Compute histograms
		XHistogram hist_in;
		hist_in.computeHistogram(values_in, bin_step, min_value, max_value);
		//cout << hist_in << endl << endl;
		XHistogram hist_out;
		hist_out.computeHistogram(values_out, bin_step, min_value, max_value);
		//cout << hist_out << endl << endl;
		// Compute distance
		float dist = XHistogram::ChiSquaredDistance(hist_in, hist_out);
		// Compute result
		result = dist;
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(MyException& e)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion boundary score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(...)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion boundary score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
}

// Compute the color_contrast_score value for an object
void SVMDetectionEvaluator::computeColorContrastScore(const Mat& frame, const cvb::CvBlob& blob, float& result)
{
	// Initialize result
	result = -1;
	// Initialize blobs
	cvb::CvBlobs blobs_in, blobs_out;
	try
	{
		// Copy parameters to local variables
		int theta = parameters.get<int>("cc_theta");
		int color_space = parameters.get<int>("cc_color_space");
		int channel = parameters.get<int>("cc_channel");
		float bin_step = parameters.get<float>("cc_bin_step");
		// Convert image
		Mat conv;
		cvtColor(frame, conv, color_space);
		//double m, M;
		//minMaxLoc(conv, &m, &M);
		//cout << "--- " << m << " --- " << M << endl;
		conv.convertTo(conv, CV_32FC3, 1.0/180.0);
		//minMaxLoc(conv, &m, &M);
		//cout << "--- " << m << " --- " << M << endl;
		// Convert blob to binary mask
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, frame.cols, frame.rows);
		// Dilate and erode mask by theta pixels
		Mat mask_in, mask_out;
		dilate(mask, mask_out, Mat(), Point(-1,-1), theta);
		erode(mask, mask_in, Mat(), Point(-1,-1), theta);
		// Compute new blobs
		blobs_in = computeBlobs(mask_in);
		blobs_out = computeBlobs(mask_out);
		// Check number of blobs
		if(blobs_in.size() == 0 || blobs_out.size() == 0)
		{
			throw MyException("Error computing inner and/or outer blobs");
		}
		// Get blobs references
		cvb::CvBlob& blob_in = *(blobs_in.begin()->second);
		cvb::CvBlob& blob_out = *(blobs_out.begin()->second);
		// Initialize value vectors
		vector<float> values_in, values_out;
		// Initialize min and max values
		float min_value=INT_MAX, max_value=INT_MIN;
		// Select inner pixels
		Point current_in = blob_in.contour.startingPoint;
		Vec3f pixel_in = conv.at<Vec3f>(current_in.y, current_in.x);
		values_in.push_back(pixel_in[channel]);
		if(pixel_in[channel] < min_value)	min_value = pixel_in[channel];
		if(pixel_in[channel] > max_value)	max_value = pixel_in[channel];
		const cvb::CvChainCodes& chain_code_in = blob_in.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_in.begin(); it != chain_code_in.end(); it++)
		{
			current_in.x += cvb::cvChainCodeMoves[*it][0];
			current_in.y += cvb::cvChainCodeMoves[*it][1];
			pixel_in = conv.at<Vec3f>(current_in.y, current_in.x);
			values_in.push_back(pixel_in[channel]);
			if(pixel_in[channel] < min_value)	min_value = pixel_in[channel];
			if(pixel_in[channel] > max_value)	max_value = pixel_in[channel];
		}
		// Select outer pixels
		Point current_out = blob_out.contour.startingPoint;
		Vec3f pixel_out = conv.at<Vec3f>(current_out.y, current_out.x);
		values_out.push_back(pixel_out[channel]);
		if(pixel_out[channel] < min_value)	min_value = pixel_out[channel];
		if(pixel_out[channel] > max_value)	max_value = pixel_out[channel];
		const cvb::CvChainCodes& chain_code_out = blob_out.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_out.begin(); it != chain_code_out.end(); it++)
		{
			current_out.x += cvb::cvChainCodeMoves[*it][0];
			current_out.y += cvb::cvChainCodeMoves[*it][1];
			pixel_out = conv.at<Vec3f>(current_out.y, current_out.x);
			values_out.push_back(pixel_out[channel]);
			if(pixel_out[channel] < min_value)	min_value = pixel_out[channel];
			if(pixel_out[channel] > max_value)	max_value = pixel_out[channel];
		}
		// Check list sizes
		if(values_in.size() == 0 || values_out.size() == 0)
		{
			throw MyException("Empty histograms for color boundary.");
		}
		// Compute histograms
		XHistogram hist_in;
		hist_in.computeHistogram(values_in, bin_step, min_value, max_value);
		//cout << hist_in << endl << endl;
		XHistogram hist_out;
		hist_out.computeHistogram(values_out, bin_step, min_value, max_value);
		//cout << hist_out << endl << endl;
		// Compute features
		float dist = XHistogram::ChiSquaredDistance(hist_in, hist_out);
		// Compute result
		result = dist;
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(MyException& e)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color contrast score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(...)
	{
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color contrast score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
}
