// Implementation of a detection evaluator

#include "bayes_detection_evaluator.h"
#include <cvblob.h>
#include <lk_motion_vector.h>
#include <motion_vector_utils.h>
#include <time_utils.h>
#include <math_utils.h>
#include <gabor_filter.h>
#include <log.h>
// Debug includes
#include <iostream>
#include <highgui.h>

using namespace cv;
using namespace std;
using namespace alg;

// Constructor
BayesDetectionEvaluator::BayesDetectionEvaluator(int frame_buffer_size) :
	Evaluator(frame_buffer_size)
{
	// Resize thread pool
	thread_pool.size_controller().resize(20);
	// Training file output
	parameters.add<string>("training_out", "train_out.txt");
	parameters.add<int>("training_label", -1);
	// Parameters for boundary differences
	parameters.add<int>("min_dist_from_border", 10);
	parameters.add<int>("theta_cc", 2);
	parameters.add<int>("theta_mb", 2);
	// Parameters for Canny filters
	parameters.add<int>("canny_t1", 80);
	parameters.add<int>("canny_t2", 60);
	// Parameters for superpixels straddling
	parameters.add<int>("sp_k", 125);
	parameters.add<int>("sp_min", 25);
	// Parameters for Gestalt-based score
	parameters.add<int>("gs_b_s", 3);
	parameters.add<int>("gs_b_k", 8);
	parameters.add<int>("gs_b_sn", 5);
	// Parameters for Gabor filters
	parameters.add<int>("num_orientations", 4);
	gabor_scales.push_back(2);
	gabor_scales.push_back(4);
	// Parameters for motion vector
	parameters.add<int>("mv_window", 3); // 3
	parameters.add<float>("mv_deriv_weight", 0); // 0
	parameters.add<int>("mv_max_level", 0); // 0
}

// Perform score computation of detection in the current frame
void BayesDetectionEvaluator::addFrame(const vector<const TrackedObject*>& tracked_objects, const Mat& frame, int frame_number, const Mat* next_frame)
{
	/*// Read parameters
	int sp_k = parameters.get<int>("sp_k");
	int sp_min = parameters.get<int>("sp_min");*/
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
		BayesDetectionScore* score = new BayesDetectionScore(&nbc);
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
		boost::thread* thread = new boost::thread(boost::bind(&BayesDetectionEvaluator::computeScoreThread, this, _1, _2, _3, _4), boost::ref(*copy), boost::ref(frame), boost::ref(*score), next_frame);
		threads.add_thread(thread);
	}	
	// Wait for all threads
	threads.join_all();
}
		
// Compute the Gestalt-based score
void BayesDetectionEvaluator::computeGestaltScore(const cvb::CvBlob& blob, int rows, int cols, float& result)
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
		Log::warning() << ShellUtils::RED << "Error computing Gestalt score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing Gestalt score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}
		
// Compute the superpixels straddling score
void BayesDetectionEvaluator::computeSuperpixelsStraddlingScore(const cv::Mat& seg, const cvb::CvBlob& blob, float& result)
{
	try
	{
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, seg.cols, seg.rows);
		result = superpixelsStraddlingScore(seg, mask);
		/*// Go through all pixels, group superpixels into single images, and at the same time check for overlap with object's blob
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, seg.cols, seg.rows);
		// Initialize superpixels mask map; each superpixel is indexed by a number computed from the RGB value used in the segmentation image
		map<int, Mat> sp_masks;
		map<int, bool> sp_straddling;
		// Go through each pixel
		for(int i=0; i<seg.rows; ++i)
		{
			for(int j=0; j<seg.cols; ++j)
			{
				// Get pixel
				const Vec3b& pixel = seg.at<Vec3b>(i,j);
				// Convert it to a number
				int sp = ((int) pixel[0]) + (((int) pixel[1]) << 8) + (((int) pixel[2]) << 16);
				// Check if the corresponding mask has been initialized
				if(sp_masks.find(sp) == sp_masks.end())
				{
					// Add black matrix
					sp_masks[sp] = Mat::zeros(seg.rows, seg.cols, CV_8UC1);
				}
				// Set the pixel in the corresponding mask
				sp_masks[sp].at<uchar>(i,j) = 255;
				// Check if it's straddling
				if(mask.at<uchar>(i,j) > 0)
				{
					// Add to straddling list
					sp_straddling[sp] = true;
				}
			}
		}
		//imshow("superpixels", seg);
		// Initialize score
		float score = 1.0f;
		// Go through all straddling pixels
		for(map<int,bool>::iterator sp_it = sp_straddling.begin(); sp_it != sp_straddling.end(); ++sp_it)
		{
			// Get superpixel mask
			Mat& sp_mask = sp_masks[sp_it->first];
			//imshow("sp", sp_mask);
			//waitKey(0);
			// Compute areas
			float area_in = binaryArea(sp_mask & mask);
			float area_out = binaryArea(sp_mask & (~mask));
			float area_mask = binaryArea(mask);
			// Update score
			score -= min(area_in, area_out)/area_mask;
		}
		// Return score
		result = score;*/
	}
	catch(MyException& e)
	{
		result = 0.1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing superpixel straddling score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result = 0.1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing superpixel straddling score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}
		
// Compute the edge density score for an object
void BayesDetectionEvaluator::computeEdgeDensityScore(const cv::Mat& frame, const cvb::CvBlob& blob, float& result)
{
	result = -1;
	try
	{
		// Read parameters
		int t1_1 = parameters.get<int>("canny_t1");
		int t1_2 = parameters.get<int>("canny_t2");
		// Convert to grayscale
		Mat gray;
		cvtColor(frame, gray, CV_BGR2GRAY);
		// Apply Canny filters
		Mat canny_1;
		Canny(gray, canny_1, t1_1, t1_2);
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
}

// Compute the internal texture score value for an object
// Returns -1 if there are not enough cells on which to apply the Gabor filter
void BayesDetectionEvaluator::computeTextureScore(const Mat& frame, const cvb::CvBlob& blob, const Mat& mask, float& result)
{
	result = -1;
	try
	{
		// Read grid cell size from parameters
		int grid_cell_size = parameters.get<int>("grid_cell_size");
		// Initialize list of cell-result vectors
		vector<vector<float> > gabor_results;
		// Divide the blob into a grid
		for(unsigned int x=blob.x; x<blob.maxx; x += grid_cell_size)
		{
			for(unsigned int y=blob.y; y<blob.maxy; y += parameters.get<int>("grid_cell_size"))
			{
				// Compute cell limits
				int max_x = (x+grid_cell_size-1 >= blob.maxx ? blob.maxx : x+grid_cell_size-1);
				int max_y = (y+grid_cell_size-1 >= blob.maxy ? blob.maxy : y+grid_cell_size-1);
				// Get sub-image
				Rect cell_rect = Rect(x, y, max_x-x+1, max_y-y+1);
				const Mat cell = frame(cell_rect);
				const Mat cell_mask = mask(cell_rect);
				// Count all non-zero elements in cell
				int valid_pixels = 0;
				for(int i=0; i<cell_mask.rows; i++)
				{
					for(int j=0; j<cell_mask.cols; j++)
					{
						if(cell_mask.at<uchar>(i,j) > 0)
						{
							valid_pixels++;
						}
					}
				}
				// Check cell is filled enough (i.e. it does not contain too many background pixels)
				if(valid_pixels < grid_cell_size*grid_cell_size*0.8)
				{
					// Skip cell
					continue;
				}
				// Apply Gabor filters
				vector<float> cell_result = GaborFilter::applyFilterSet(cell, gabor_scales, parameters.get<int>("num_orientations"));//, 3);
				// Add to result set
				gabor_results.push_back(cell_result);
			}
		}
		// Check number of output vectors
		if(gabor_results.size() < 3)
		{
			// Notify the caller that something went wrong
			result = -1;
		}
		// Convert vectors to matrix (each vector -> 1 row)
		int row_size = gabor_results[0].size();
		Mat result_mat(gabor_results.size(), row_size, CV_32F);
		for(unsigned int i=0; i<gabor_results.size(); i++)
		{
			for(int j=0; j<row_size; j++)
			{
				result_mat.at<float>(i,j) = gabor_results[i][j];
			}
		}
		// Compute mean vector
		Mat mean = Mat::zeros(1, row_size, CV_32F);
		for(int i=0; i<result_mat.rows; i++)
		{
			for(int j=0; j<row_size; j++)
			{
				mean.at<float>(0,j) += result_mat.at<float>(i,j);
			}
		}
		for(int j=0; j<row_size; j++)
		{
			mean.at<float>(0,j) /= result_mat.rows;
		}
		// Compute distances between mean vector and all others
		vector<float> distances;
		for(int i=0; i<result_mat.rows; i++)
		{
			Mat result = result_mat.row(i);
			distances.push_back(norm(mean, result));
		}
		// Compute mean distance
		float texture_avg_mean_distance = 0.0f;
		for(unsigned int i=0; i<distances.size(); i++)
		{
			texture_avg_mean_distance += distances[i];
		}
		texture_avg_mean_distance /= distances.size();
		// Return result
		result = texture_avg_mean_distance;
	}
	catch(MyException& e)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing texture score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing texture score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}

// Compute the color internal score for an object
void BayesDetectionEvaluator::computeColorInternalScore(const Mat& frame, const cvb::CvBlob& blob, float* result)
{
	// Initialize result to invalid value
	result[0] = result[1] = result[2] = -1;
	try
        {
                // Rebuild blob mask
                Mat mask = drawBlob(blob, true);
                // Convert to destination color space
                Mat conv;
                cvtColor(frame, conv, CV_BGR2HSV);
                // Initialize lists
                vector<float> a_list;
                vector<float> b_list;
                vector<float> c_list;
                // Go through all mask pixels
                for(unsigned int x=blob.x; x<=blob.maxx; x++)
                {
                        for(unsigned int y=blob.y; y<=blob.maxy; y++)
                        {
                                // Check mask
                                if(mask.at<uchar>(y,x) > 0)
                                {
                                        // Read motion vector in this point
                                        Vec3b pixel = conv.at<Vec3b>(y,x);
                                        // Add to lists
                                        a_list.push_back(pixel[0]);
                                        b_list.push_back(pixel[1]);
                                        c_list.push_back(pixel[2]);
                                }
                        }
                }
                // Compute means and std devs
                float a_mean, a_std;
                float b_mean, b_std;
                float c_mean, c_std;
                MathUtils::computeMeanAndStdDev<float>(a_list, a_mean, a_std);
                MathUtils::computeMeanAndStdDev<float>(b_list, b_mean, b_std);
                MathUtils::computeMeanAndStdDev<float>(c_list, c_mean, c_std);
                // Check for errors
                if((a_mean == 0 && a_std == 0) || (b_mean == 0 && b_std == 0) || (c_mean == 0 && c_std == 0))
                {
                        throw MyException("error");
                }
                // Set result
                result[0] = a_std;
                result[1] = b_std;
                result[2] = c_std;
        }
	catch(MyException& e)
	{
		result[0] = result[1] = result[2] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color internal score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result[0] = result[1] = result[2] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color internal score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}

// Compute the motion internal score for an object
void BayesDetectionEvaluator::computeMotionInternalScore(const Mat& motion_vector, const cvb::CvBlob& blob, const Rect& search_area, float* result)
{
	// Initialize result to invalid value
	result[0] = result[1] = -1;
	try
        {
                // Rebuild blob mask
                Mat mask = drawBlob(blob, true);
                // Initialize lists of delta x and delta y
                vector<float> dx_list;
                vector<float> dy_list;
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
                                        dx_list.push_back(mv_point[2]);
                                        dy_list.push_back(mv_point[3]);
                                }
                        }
                }
                // Compute means and std devs
                float dx_mean, dx_std;
                float dy_mean, dy_std;
                MathUtils::computeMeanAndStdDev<float>(dx_list, dx_mean, dx_std);
                MathUtils::computeMeanAndStdDev<float>(dy_list, dy_mean, dy_std);
                // Check for errors
                if((dx_mean == 0 && dx_std == 0) || (dy_mean == 0 && dy_std == 0))
                {
                        throw MyException("error");
                }
                // Set result
                result[0] = dx_std;
                result[1] = dy_std;
        }
	catch(MyException& e)
	{
		result[0] = result[1] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion internal score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
	catch(...)
	{
		result[0] = result[1] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion internal score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
	}
}
	
// Compute the motion_boundary_score value for an object
void BayesDetectionEvaluator::computeMotionBoundaryScore(const Mat& motion_vector, const cvb::CvBlob& blob, float* result)
{
	// Initialize result
	result[0] = result[1] = -1;
	// Initialize blobs
	cvb::CvBlobs blobs_in;
	cvb::CvBlobs blobs_out;
	try
	{
		// Copy parameters to local variables
		int theta = parameters.get<int>("theta_mb");
		int min_dist = parameters.get<int>("min_dist_from_border");
		// Convert blob to binary mask
		Mat mask = blobToMat(blob, CV_POSITION_ORIGINAL, true, motion_vector.cols, motion_vector.rows);
		// Dilate and erode mask by theta pixels
		Mat mask_in, mask_out;
		dilate(mask, mask_out, Mat(), Point(-1,-1), theta);
		erode(mask, mask_in, Mat(), Point(-1,-1), theta);
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
		vector<float> dx_list_in, dy_list_in;
		vector<float> dx_list_out, dy_list_out;
		// Initialize dx and dy ranges
		float min_dx = INT_MAX, min_dy = INT_MAX;
		float max_dx = 0, max_dy = 0;
		// Select inner pixels, checking for minimum distance from the border
		Point current_in = blob_in.contour.startingPoint;
		Vec4d mv_in = motion_vector.at<Vec4d>(current_in.y, current_in.x);
		if(!(current_in.x < min_dist || current_in.x > motion_vector.cols-min_dist || current_in.y < min_dist || current_in.y > motion_vector.rows-min_dist))
		{
			if(mv_in[2] < min_dx)	min_dx = mv_in[2];
			if(mv_in[2] > max_dx)	max_dx = mv_in[2];
			if(mv_in[3] < min_dy)	min_dy = mv_in[3];
			if(mv_in[3] > max_dy)	max_dy = mv_in[3];
			dx_list_in.push_back(mv_in[2]);
			dy_list_in.push_back(mv_in[3]);
		}
		// Get chain code contour
		const cvb::CvChainCodes& chain_code_in = blob_in.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_in.begin(); it != chain_code_in.end(); it++)
		{
			current_in.x += cvb::cvChainCodeMoves[*it][0];
			current_in.y += cvb::cvChainCodeMoves[*it][1];
			Vec4d mv_in = motion_vector.at<Vec4d>(current_in.y, current_in.x);
			if(!(current_in.x < min_dist || current_in.x > motion_vector.cols-min_dist || current_in.y < min_dist || current_in.y > motion_vector.rows-min_dist))
			{
				if(mv_in[2] < min_dx)	min_dx = mv_in[2];
				if(mv_in[2] > max_dx)	max_dx = mv_in[2];
				if(mv_in[3] < min_dy)	min_dy = mv_in[3];
				if(mv_in[3] > max_dy)	max_dy = mv_in[3];
				dx_list_in.push_back(mv_in[2]);
				dy_list_in.push_back(mv_in[3]);
			}
		}
		// Select outer pixels
		Point current_out = blob_out.contour.startingPoint;
		Vec4d mv_out = motion_vector.at<Vec4d>(current_out.y, current_out.x);
		if(!(current_out.x < min_dist || current_out.x > motion_vector.cols-min_dist || current_out.y < min_dist || current_out.y > motion_vector.rows-min_dist))
		{
			if(mv_out[2] < min_dx)	min_dx = mv_out[2];
			if(mv_out[2] > max_dx)	max_dx = mv_out[2];
			if(mv_out[3] < min_dy)	min_dy = mv_out[3];
			if(mv_out[3] > max_dy)	max_dy = mv_out[3];
			dx_list_out.push_back(mv_out[2]);
			dy_list_out.push_back(mv_out[3]);
		}
		// Get chain code contour
		const cvb::CvChainCodes& chain_code_out = blob_out.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_out.begin(); it != chain_code_out.end(); it++)
		{
			current_out.x += cvb::cvChainCodeMoves[*it][0];
			current_out.y += cvb::cvChainCodeMoves[*it][1];
			Vec4d mv_out = motion_vector.at<Vec4d>(current_out.y, current_out.x);
			if(!(current_out.x < min_dist || current_out.x > motion_vector.cols-min_dist || current_out.y < min_dist || current_out.y > motion_vector.rows-min_dist))
			{
				if(mv_out[2] < min_dx)	min_dx = mv_out[2];
				if(mv_out[2] > max_dx)	max_dx = mv_out[2];
				if(mv_out[3] < min_dy)	min_dy = mv_out[3];
				if(mv_out[3] > max_dy)	max_dy = mv_out[3];
				dx_list_out.push_back(mv_out[2]);
				dy_list_out.push_back(mv_out[3]);
			}
		}
		// Check list sizes
		if(dx_list_in.size() == 0 || dx_list_out.size() == 0 || dy_list_in.size() == 0 || dy_list_out.size() == 0)
		{
			throw MyException("Empty histograms for motion boundary.");
		}
		// Normalize value lists
		vector<uchar> dx_list_in_uchar;
		vector<uchar> dy_list_in_uchar;
		vector<uchar> dx_list_out_uchar;
		vector<uchar> dy_list_out_uchar;
		for(vector<float>::iterator it = dx_list_in.begin(); it != dx_list_in.end(); it++)
		{
			float value = *it;
			uchar norm_value = (value - min_dx)*255/(max_dx - min_dx);
			dx_list_in_uchar.push_back(norm_value);
		}
		for(vector<float>::iterator it = dx_list_out.begin(); it != dx_list_out.end(); it++)
		{
			float value = *it;
			uchar norm_value = (value - min_dx)*255/(max_dx - min_dx);
			dx_list_out_uchar.push_back(norm_value);
		}
		for(vector<float>::iterator it = dy_list_in.begin(); it != dy_list_in.end(); it++)
		{
			float value = *it;
			uchar norm_value = (value - min_dy)*255/(max_dy - min_dy);
			dy_list_in_uchar.push_back(norm_value);
		}
		for(vector<float>::iterator it = dy_list_out.begin(); it != dy_list_out.end(); it++)
		{
			float value = *it;
			uchar norm_value = (value - min_dy)*255/(max_dy - min_dy);
			dy_list_out_uchar.push_back(norm_value);
		}
		// Compute histograms
		Histogram dx_in(dx_list_in_uchar), dy_in(dy_list_in_uchar);
		Histogram dx_out(dx_list_out_uchar), dy_out(dy_list_out_uchar);
		// Compute features
		float dist_dx = Histogram::ChiSquaredDistance(dx_in, dx_out);
		float dist_dy = Histogram::ChiSquaredDistance(dy_in, dy_out);
		// Compute result
		result[0] = dist_dx;
		result[1] = dist_dy;
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(MyException& e)
	{
		result[0] = result[1] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion boundary score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(...)
	{
		result[0] = result[1] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing motion boundary score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
}

// Compute the color_contrast_score value for an object
void BayesDetectionEvaluator::computeColorContrastScore(const Mat& frame, const cvb::CvBlob& blob, float* result)
{
	// Initialize result
	result[0] = result[1] = result[2] = -1;
	// Initialize blobs
	cvb::CvBlobs blobs_in, blobs_out;
	try
	{
		// Copy parameters to local variables
		int theta = parameters.get<int>("theta_cc");
		int min_dist = parameters.get<int>("min_dist_from_border");
		// Convert image to LAB
		Mat lab;
		cvtColor(frame, lab, CV_BGR2HSV);
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
		vector<uchar> L_list_in, a_list_in, b_list_in;
		vector<uchar> L_list_out, a_list_out, b_list_out;
		// Select inner pixels, checking for minimum distance from the border
		Point current_in = blob_in.contour.startingPoint;
		Vec3b pixel_in = lab.at<Vec3b>(current_in.y, current_in.x);
		if(!(current_in.x < min_dist || current_in.x > frame.cols-min_dist || current_in.y < min_dist || current_in.y > frame.rows-min_dist))
		{
			L_list_in.push_back(pixel_in[0]);
			a_list_in.push_back(pixel_in[1]);
			b_list_in.push_back(pixel_in[2]);
		}
		// Get chain code contour
		const cvb::CvChainCodes& chain_code_in = blob_in.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_in.begin(); it != chain_code_in.end(); it++)
		{
			current_in.x += cvb::cvChainCodeMoves[*it][0];
			current_in.y += cvb::cvChainCodeMoves[*it][1];
			pixel_in = lab.at<Vec3b>(current_in.y, current_in.x);
			if(!(current_in.x < min_dist || current_in.x > frame.cols-min_dist || current_in.y < min_dist || current_in.y > frame.rows-min_dist))
			{
				L_list_in.push_back(pixel_in[0]);
				a_list_in.push_back(pixel_in[1]);
				b_list_in.push_back(pixel_in[2]);
			}
		}
		// Select outer pixels
		Point current_out = blob_out.contour.startingPoint;
		Vec3b pixel_out = lab.at<Vec3b>(current_out.y, current_out.x);
		if(!(current_out.x < min_dist || current_out.x > frame.cols-min_dist || current_out.y < min_dist || current_out.y > frame.rows-min_dist))
		{
			L_list_out.push_back(pixel_out[0]);
			a_list_out.push_back(pixel_out[1]);
			b_list_out.push_back(pixel_out[2]);
		}
		// Get chain code contour
		const cvb::CvChainCodes& chain_code_out = blob_out.contour.chainCode;
		for(cvb::CvChainCodes::const_iterator it=chain_code_out.begin(); it != chain_code_out.end(); it++)
		{
			current_out.x += cvb::cvChainCodeMoves[*it][0];
			current_out.y += cvb::cvChainCodeMoves[*it][1];
			pixel_out = lab.at<Vec3b>(current_out.y, current_out.x);
			if(!(current_out.x < min_dist || current_out.x > frame.cols-min_dist || current_out.y < min_dist || current_out.y > frame.rows-min_dist))
			{
				L_list_out.push_back(pixel_out[0]);
				a_list_out.push_back(pixel_out[1]);
				b_list_out.push_back(pixel_out[2]);
			}
		}
		// Check list sizes
		if(L_list_in.size() == 0 || L_list_out.size() == 0 || a_list_in.size() == 0 || a_list_out.size() == 0 || b_list_in.size() == 0 || b_list_out.size() == 0)
		{
			throw MyException("Empty histograms for color boundary.");
		}
		// Compute histograms
		Histogram L_in(L_list_in), a_in(a_list_in), b_in(b_list_in);
		Histogram L_out(L_list_out), a_out(a_list_out), b_out(b_list_out);
		// Compute features
		float dist_L = Histogram::ChiSquaredDistance(L_in, L_out);
		float dist_a = Histogram::ChiSquaredDistance(a_in, a_out);
		float dist_b = Histogram::ChiSquaredDistance(b_in, b_out);
		// Compute result
		result[0] = dist_L;
		result[1] = dist_a;
		result[2] = dist_b;
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(MyException& e)
	{
		result[0] = result[1] = result[2] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color contrast score: " << e.what() << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
	catch(...)
	{
		result[0] = result[1] = result[2] = -1;
		log_mutex.lock();
		Log::warning() << ShellUtils::RED << "Error computing color contrast score." << ShellUtils::NORMAL << endl;
		log_mutex.unlock();
		// Free blobs
		cvReleaseBlobs(blobs_in);
		cvReleaseBlobs(blobs_out);
	}
}
