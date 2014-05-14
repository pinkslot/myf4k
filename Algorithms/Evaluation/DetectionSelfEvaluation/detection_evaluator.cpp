// Implementation of a detection evaluator based on Erdem's paper
// FIXME samplecontour, hobihu-2, frame 2902
// FIXME texture difference score

#include "detection_evaluator.h"
#include "detection_score.h"
#include <cvblob.h>
#include <lk_motion_vector.h>
#include <motion_vector_utils.h>
#include <gabor_filter.h>
#include <log.h>
// Debug includes
#include <iostream>
#include <highgui.h>

using namespace cv;
using namespace std;
using namespace alg;

// Constructor:
// - frame_buffer_size: number of previous frames to be stored, in order to compare them with objects recognized in the current frame but which have been missing for a while
DetectionEvaluator::DetectionEvaluator(int frame_buffer_size) : Evaluator(frame_buffer_size)
{
	// Set parameters
	// Weights
	parameters.add<float>("weight_cb", 1.0f);
	parameters.add<float>("weight_mb", 1.0f);
	parameters.add<float>("weight_mi", 1.0f);
	parameters.add<float>("weight_ci", 1.0f);
	parameters.add<float>("weight_td", 0.0f);
	// Parameters for boundary differences (see Erdem's paper)
	parameters.add<float>("l", 5.0f);
	parameters.add<float>("m", 2.0f);
	parameters.add<int>("contour_samples", 25);
	// Parameters for Gabor filters
	parameters.add<int>("num_orientations", 4);
	parameters.add<int>("num_scales", 2);
	parameters.add<int>("min_scale", 2);
	parameters.add<int>("max_scale", 4);
	// Parameters for internal grids
	parameters.add<int>("grid_cell_size", 6);
}

// Perform contour sampling, for each sample draw a normal segment with length "l" and get the two ends of each segment
SampledContour DetectionEvaluator::sampleContour(const cvb::CvBlob& blob)//, Mat& drawn)
{
	// Initialize result
	SampledContour result;
	// Get contour as chain code
	Point current = blob.contour.startingPoint;
	const cvb::CvChainCodes& chain_code = blob.contour.chainCode;
	// Convert chain code to point list (easier to manage...)
	vector<Point> contour;
	contour.push_back(current);
	int num_points = chain_code.size();
	// Check minimum number of points
	if(num_points > 0 && num_points < 15)
	{
		Log::warning() << "DetectionEvaluator::sampleContour(): Too few contour points (" << num_points << "), returning contour with single dummy point" << endl;
		pair<Point, Point> elem;
		elem.first = current;
		elem.second = current;
		result.push_back(elem);
		return result;
	}
	for(cvb::CvChainCodes::const_iterator it=chain_code.begin(); it != chain_code.end(); it++)
	{
		current.x += cvb::cvChainCodeMoves[*it][0];
		current.y += cvb::cvChainCodeMoves[*it][1];
		contour.push_back(current);
	}
	// Compute the sampling rate
	int sampling_rate = num_points/parameters.get<int>("contour_samples");
	if(sampling_rate == 0)
		sampling_rate = num_points;
	// Read each contour sample
	cout << "calling function, num points: " << num_points << endl;
	for(int i=0; i<num_points; i += sampling_rate)
	{
		//cout << "sampling point " << i << endl;
		// Get target point
		//drawn.at<Vec3b>(contour[i].y, contour[i].x) = Vec3b(0,0,255);
		Point2f curr = contour[i];
		// Get previous and next indexes
		int prev_i = (i-5 < 0 ? num_points-1-(5-i) : i-5);
		int next_i = (i+5) % num_points;
		// Get previous and next points
		Point2f prev = contour[prev_i];
		Point2f next = contour[next_i];
		// Get ends of normal segment passing by point i
		// Basically, get the line between prev and next, find its orthogonal passing by point i, and find the points belonging to this line which are l pixels far from point i
		float m1 = (next.y - prev.y)/(next.x - prev.x);
		// Check NaN
		if(m1 != m1)
		{
			continue;
		}
		float m = (m1 != 0.0f) ? -1/m1 : 100;
		float q = -m*curr.x + curr.y;
		float k = parameters.get<int>("l")/sqrt(m*m+1);
		float xe1 = curr.x - k;
		float xe2 = curr.x + k;
		Point pe1(xe1, m*xe1+q);
		Point pe2(xe2, m*xe2+q);
		//line(drawn, pe1, pe2, CV_RGB(255,0,0));
		// Add points to output vector
		pair<Point, Point> elem;
		elem.first = pe1;
		elem.second = pe2;
		result.push_back(elem);
	}
	// If result is empty, put a dummy pair
	if(result.size() == 0)
	{
		pair<Point, Point> elem;
		elem.first = current;
		elem.second = current;
		result.push_back(elem);
	}
	// Return result
	return result;
}


// Perform score computation of detection in the current frame
void DetectionEvaluator::addFrame(const vector<const TrackedObject*>& tracked_objects, const Mat& frame, int frame_number)
{
	// Add current frame to buffer
	frame_buffer->add(frame.clone());
	// Process each tracked object
	//int debugi = 0;
	for(vector<const TrackedObject*>::const_iterator it = tracked_objects.begin(); it != tracked_objects.end(); it++)
	{
		//cout << "object: " << debugi++ << endl;
		// Get reference
		const TrackedObject& tracked_object = **it;
		// Check that the object appears in this frame
		if(tracked_object.lastAppearance() != frame_number)
		{
			// Skip object
			continue;
		}
		// If we don't have a ScoreHistory for this object, create it
		if(objects.find(tracked_object.id) == objects.end())
		{
			ScoreHistory* history = new ScoreHistory();
			// Add history to object list
			objects[tracked_object.id] = history;
		}
		ScoreHistory* history = objects[tracked_object.id];
		// Get object's blob
		cvb::CvBlob blob = tracked_object.currentRegion();
		//if(frame_number == 348)
		//{
		//	Mat drawn_blob = drawBlob(blob, true);
		//	imshow("aaa", drawn_blob);
		//	waitKey(0);
		//}
		// Create a new score for this object
		DetectionScore* score = new DetectionScore();
		// Set weight values, with motion-vector-related scores to 0, since we're not sure we can computer them until we find another detection
		score->setWeights(parameters.get<float>("weight_cb"), 0, 0, parameters.get<float>("weight_ci"), parameters.get<float>("weight_td"));
		// Sample contour and get segment ends
		//cout << 5;cout.flush();
		SampledContour sampled_contour = sampleContour(blob);
		// Save sampled contour
		score->setSampledContour(sampled_contour);
		// Compute color_boundary_score
		//cout << 7;cout.flush();
		score->color_boundary_score = computeColorBoundaryScore(frame, sampled_contour);//, blob);
		cout << "cbs: " << score->color_boundary_score << endl;
		// Compute object's binary mask, for the histogram
		//cout << 8;cout.flush();
		Mat mask = drawBlob(blob, true, frame.cols, frame.rows);
		// Compute color internal_score
		//cout << "comp int score" << endl;
		score->color_internal_score = computeColorInternalScore(frame, blob, mask);//, blob);
		cout << "cis: " << score->color_internal_score << endl;
		// Get grayscale frame
		Mat frame_gs;
		cvtColor(frame, frame_gs, CV_BGR2GRAY);
		// Compute texture score
		score->texture_diff_score = computeTextureScore(frame_gs, blob, mask);
		cout << "tds: " << score->texture_diff_score << endl;
		if(score->texture_diff_score < 0)
		{
			// Clear weight
			score->w_td = 0;
		}
		// Get the frame number of the previous detection of this object, if available
		int prev_frame = -1;
		DetectionScore* prev_score = NULL;
		cvb::CvBlob prev_blob;
		if(history->numScores() > 0)
		{
			// Read frame number
			prev_frame = *(tracked_object.frameList().end() - 2);
			// Get previous score
			prev_score = (DetectionScore*) history->getScore(prev_frame);
			// Get previous blob
			prev_blob = *(tracked_object.regionList().end() - 2);
		}
		// Initialize motion vector
		LKMotionVector mv;
		mv.window_size = 3;
		mv.derivative_weight = 0;
		mv.max_level = 0;
		// If we have a previous frame, check the buffer contains it
		if(prev_frame > 0)
		{
			// Compute the number of the oldest stored frame in the buffer
			int oldest_stored_frame = frame_number - frame_buffer->maxSize() + 1;
			// Check that we're inside that limit
			if(prev_frame >= oldest_stored_frame)
			{
				// Compute the rectangle containing both detections
				Rect prev_rect(prev_blob.x, prev_blob.y, prev_blob.width(), prev_blob.height());
				Rect curr_rect(blob.x, blob.y, blob.width(), blob.height());
				Rect join_rect = prev_rect | curr_rect;
				// Increase rectangle size to include sampled contour segments
				join_rect -= Point(parameters.get<int>("l")+parameters.get<int>("m")+5, parameters.get<int>("l")+parameters.get<int>("m")+5);
				join_rect += Size(2*(parameters.get<int>("l")+parameters.get<int>("m")+5), 2*(parameters.get<int>("l")+parameters.get<int>("m")+5));
				// Check limits
				if(join_rect.x < 0)	join_rect.x = 0;
				if(join_rect.y < 0)	join_rect.y = 0;
				if(join_rect.x + join_rect.width - 1 >= frame.cols)	join_rect.width = frame.cols - join_rect.x;
				if(join_rect.y + join_rect.height - 1 >= frame.rows)	join_rect.height = frame.rows - join_rect.y;
				// Compute motion vector
				const Mat& prev_mat = frame_buffer->get(frame_number-prev_frame);
				const Mat& curr_mat = frame_buffer->get(0);
				mv.addFrame(prev_mat);
				Mat motion_vector = mv.addFrame(curr_mat, join_rect);
				prev_score->motion_boundary_score = computeMotionBoundaryScore(motion_vector, prev_score->getSampledContour());
				cout << "prev mbs: " << prev_score->motion_boundary_score << endl;
				// Rebuild blob mask
				Mat mask = drawBlob(prev_blob, true);
				// Compute motion internal score
				prev_score->motion_internal_score = computeMotionInternalScore(motion_vector, prev_blob, mask, join_rect);
				cout << "prev mis: " << prev_score->motion_internal_score << endl;
				// Set motion weights to the default value
				prev_score->w_mb = parameters.get<float>("weight_mb");
				prev_score->w_mi = parameters.get<float>("weight_mi");
			}
		}
		// Add new score to history
		history->addScore(score, frame_number);
		Log::info() << "detection score: " << score->value();
		if(prev_score != NULL) Log::info(0) << " (prev score updated: " << prev_score->value() << ")";
		Log::info(0) << endl;
		//cout << "curr detection score: " << score->value() << endl;
	}
	//cout << "finished" << endl;
}

// Compute the internal texture score value for an object
// Returns -1 if there are not enough cells on which to apply the Gabor filter
float DetectionEvaluator::computeTextureScore(const Mat& frame, const cvb::CvBlob& blob, const Mat& mask)
{
	// Read grid cell size from parameters
	int grid_cell_size = parameters.get<int>("grid_cell_size");
	// Initialize list of cell-result vectors
	vector<vector<float> > gabor_results;
	//imshow("f", frame);
	//imshow("m", mask);
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
			vector<float> cell_result = GaborFilter::applyFilterSet(cell, parameters.get<int>("min_scale"), parameters.get<int>("max_scale"), parameters.get<int>("num_scales"), parameters.get<int>("num_orientations"), 1, 0.5);//, 3);
			// Add to result set
			gabor_results.push_back(cell_result);
		}
	}
	// Check number of output vectors
	if(gabor_results.size() < 3)
	{
		// Notify the caller that something went wrong
		return -1;
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
	float mean_distance = 0.0f;
	for(unsigned int i=0; i<distances.size(); i++)
	{
		mean_distance += distances[i];
	}
	mean_distance /= distances.size();
	//cout << "texture distance: " << mean_distance << endl;
	// Compute covariance matrix
	//Mat covar, mean_mat;
	//calcCovarMatrix(result_mat, covar, mean_mat, CV_COVAR_NORMAL | CV_COVAR_SCALE | CV_COVAR_ROWS);
	// Read variances
	/*float var_sum = 0.0f;
	cout << "texture var: ";
	for(int i=0; i<covar.rows; i++)
	{
		var_sum += covar.at<double>(i,i);
		cout << covar.at<double>(i,i) << " ";
	}
	cout << endl;
	// Compute variance mean
	float var_mean = var_sum/covar.rows;
	cout << "texture var mean: " << var_mean << endl;
	// Compute variance penalty (to subtract from the maximum score)
	float penalty = var_mean/1000.0f;
	// Compute maximum score
	float max_score = 1.0f;// - ((float)valid_cells)/((float)num_cells)*0.10;
	// Compute final score
	float score = max_score - penalty;
	// Process results*/
	return 1;
}

// Compute the color internal score for an object
float DetectionEvaluator::computeColorInternalScore(const Mat& frame, const cvb::CvBlob& blob, const Mat& mask)
{
	try
	{
		// Read grid cell size
		int grid_cell_size = parameters.get<int>("grid_cell_size");
		//Mat drawn = Mat::zeros(motion_vector.rows, motion_vector.cols, CV_8UC3);
		//Mat mv_drawn = Mat::zeros(motion_vector.rows, motion_vector.cols, CV_64FC4);
		// Initialize list of average vectors
		vector<Vec3f> avg_colors;
		// Cell counter
		int num_cells = 0;
		// Valid cell counter
		//int valid_cells = 0;
		// Divide the blob into a grid
		for(unsigned int x=blob.x; x<blob.maxx; x += grid_cell_size)
		{
			for(unsigned int y=blob.y; y<blob.maxy; y += grid_cell_size)
			{
				// Increase number of cells
				num_cells++;
				//Mat drawn = Mat::zeros(motion_vector.rows, motion_vector.cols, CV_8UC3);
				int max_x = (x+grid_cell_size-1 >= blob.maxx ? blob.maxx : x+grid_cell_size-1);
				int max_y = (y+grid_cell_size-1 >= blob.maxy ? blob.maxy : y+grid_cell_size-1);
				// Initialize the average color for this cell
				Vec3f avg_color(0.0f, 0.0f, 0.0f);
				// Vector counter
				int num_vectors = 0;
				// Add all motion vectors
				for(int i=y; i<=max_y; i++)
				{
					for(int j=x; j<=max_x; j++)
					{
						// Check mask
						if(mask.at<uchar>(i,j) > 0)
						{
							// Increase total count
							num_vectors++;
							// Read pixel in this point
							Vec3b color = frame.at<Vec3b>(i,j);
							// Add color
							avg_color[0] = avg_color[0] + color[0];
							avg_color[1] = avg_color[1] + color[1];
							avg_color[2] = avg_color[2] + color[2];
						}
					}
				}
				if(num_vectors > 0)
				{
					// Divide by number of elements
					avg_color[0] = avg_color[0]/num_vectors;
					avg_color[1] = avg_color[1]/num_vectors;
					avg_color[2] = avg_color[2]/num_vectors;
					//cout << "cell: " << avg_mv[0] << " " << avg_mv[1] << endl;
					// Increase number of valid cells
					//valid_cells++;
					// Add to list
					avg_colors.push_back(avg_color);
				}
				//cout << "last cell: " << num_vectors << " elements (" << valid_vectors << " valid), avg=" << avg_mv[0] << "," << avg_mv[1] << endl;
			}
		}
		//cout << "Total: " << num_cells << " cells (" << valid_cells << " valid)" << endl;
		// Convert average vectors to matrix (each vector -> 1 row)
		Mat avg_mat(avg_colors.size(), 3, CV_32F);
		for(unsigned int i=0; i<avg_colors.size(); i++)
		{
			avg_mat.at<float>(i,0) = avg_colors[i][0];
			avg_mat.at<float>(i,1) = avg_colors[i][1];
			avg_mat.at<float>(i,2) = avg_colors[i][2];
		}
		// Compute mean vector
		Mat mean = Mat::zeros(1, 3, CV_32F);
		for(int i=0; i<avg_mat.rows; i++)
		{
			for(int j=0; j<3; j++)
			{
				mean.at<float>(0,j) += avg_mat.at<float>(i,j);
			}
		}
		for(int j=0; j<3; j++)
		{
			mean.at<float>(0,j) /= avg_mat.rows;
		}
		// Compute distances between mean vector and all others
		vector<float> distances;
		for(int i=0; i<avg_mat.rows; i++)
		{
			Mat result = avg_mat.row(i);
			distances.push_back(norm(mean, result));
		}
		// Compute mean distance
		float mean_distance = 0.0f;
		for(unsigned int i=0; i<distances.size(); i++)
		{
			mean_distance += distances[i];
		}
		mean_distance /= distances.size();
		//cout << "color internal distance: " << mean_distance << endl;
		// Compute final score
		float score = (-3.0f/5.0f*mean_distance+125)/100.0f;//(110.0f - mean_distance)/110.0f;
		// Check limits
		if(score > 1.0f) score = 1.0f;
		if(score < 0.0f) score = 0.0f;
		// Check NaN
		if(score != score)
		{
			Log::warning() << "DetectionEvaluator::computeColorInternalScore(): got NaN, returning 0.5" << endl;
			return 0.5;
		}
		// Return score
		return score;
	}
	catch(...)
	{
		Log::warning() << "Error while computing color internal score for detection certainty, returning 0.5." << endl;
		return 0.5f;
	}
}

// Compute the motion internal score for an object
float DetectionEvaluator::computeMotionInternalScore(const Mat& motion_vector, const cvb::CvBlob& blob, const Mat& mask, const Rect& search_area)
{
	try
	{
		// Read grid cell size
		int grid_cell_size = parameters.get<int>("grid_cell_size");
		Mat drawn = Mat::zeros(motion_vector.rows, motion_vector.cols, CV_8UC3);
		Mat mv_drawn = Mat::zeros(motion_vector.rows, motion_vector.cols, CV_64FC4);
		// Initialize list of average vectors
		vector<Vec2f> avg_mvs;
		// Cell counter
		int num_cells = 0;
		// Valid cell counter
		int valid_cells = 0;
		// Divide the blob into a grid
		for(unsigned int x=blob.x; x<blob.maxx; x += grid_cell_size)
		{
			for(unsigned int y=blob.y; y<blob.maxy; y += grid_cell_size)
			{
				// Increase number of cells
				num_cells++;
				//Mat drawn = Mat::zeros(motion_vector.rows, motion_vector.cols, CV_8UC3);
				int max_x = (x+grid_cell_size-1 >= blob.maxx ? blob.maxx : x+grid_cell_size-1);
				int max_y = (y+grid_cell_size-1 >= blob.maxy ? blob.maxy : y+grid_cell_size-1);
				// Initialize the average motion vector for this cell
				Vec2f avg_mv(0.0f, 0.0f);
				// Vector counter
				int num_vectors = 0;
				// Valid vector counter
				int valid_vectors = 0;
				// Add all motion vectors
				for(int i=y; i<=max_y; i++)
				{
					for(int j=x; j<=max_x; j++)
					{
						// Check mask
						if(mask.at<uchar>(i,j) > 0)
						{
							// Increase total count
							num_vectors++;
							// Read motion vector in this point
							Vec4d mv_point = motion_vector.at<Vec4d>(i,j);
							// Check that the destination point is inside the searching area, otherwise ignore this value
							Point dest_point(j + mv_point[2], i + mv_point[3]);
							if(dest_point.x >= search_area.x && dest_point.x < search_area.x+search_area.width && dest_point.y >= search_area.y && dest_point.y < search_area.y+search_area.height
							// Also, check for minimum modulus
							//sqrt(dest_point.x*dest_point.x + dest_point.y*dest_point.y) > 5)
							)
							//if(dest_point.x >= 0 && dest_point.x < motion_vector.cols && dest_point.y >= 0 && dest_point.y <= motion_vector.rows)
							{
								// Increase valid count
								valid_vectors++;
								// Add vector
								avg_mv[0] = avg_mv[0] + mv_point[2];
								avg_mv[1] = avg_mv[1] + mv_point[3];
							}
						}
					}
				}
				if(valid_vectors > 0)
				{
					// Divide by number of elements
					avg_mv[0] = avg_mv[0]/valid_vectors;
					avg_mv[1] = avg_mv[1]/valid_vectors;
					//cout << "cell: " << avg_mv[0] << " " << avg_mv[1] << endl;
					// Increase number of valid cells
					valid_cells++;
					// Add to list
					avg_mvs.push_back(avg_mv);
					// Set all pixels in the cell to the average
					/*for(int i=y; i<=max_y; i++)
					{
						for(int j=x; j<=max_x; j++)
						{
							if(mask.at<uchar>(i,j) > 0)
							{
								mv_drawn.at<Vec4d>(i,j)[2] = avg_mv[0];
								mv_drawn.at<Vec4d>(i,j)[3] = avg_mv[1];
							}
						}
					}*/

				}
				//cout << "last cell: " << num_vectors << " elements (" << valid_vectors << " valid), avg=" << avg_mv[0] << "," << avg_mv[1] << endl;
			}
		}
		//cout << "Total: " << num_cells << " cells (" << valid_cells << " valid)" << endl;
		// Convert average vectors to matrix (each vector -> 1 row)
		Mat avg_mat(avg_mvs.size(), 2, CV_64F);
		for(unsigned int i=0; i<avg_mvs.size(); i++)
		{
			avg_mat.at<double>(i,0) = avg_mvs[i][0];
			avg_mat.at<double>(i,1) = avg_mvs[i][1];
		}
		// If all values are below a certain value, amplify them
		Mat abs_mat = abs(avg_mat);
		//double min_val, max_val;
		//minMaxLoc(abs_mat, &min_val, &max_val);
		//cout << "min: " << min_val << ", max: " << max_val << endl;
		Scalar mean_val = mean(abs_mat);
		double threshold = 10.0;
		if(mean_val[0] < threshold)
		{
			// Compute amplification factor
			double ampl_factor = threshold/mean_val[0];
			// Amplify all values
			avg_mat = avg_mat*ampl_factor;
		}
		// Compute covariance matrix
		Mat covar, mean;
		calcCovarMatrix(avg_mat, covar, mean, CV_COVAR_NORMAL | CV_COVAR_SCALE | CV_COVAR_ROWS);
		// Read variances
		float var_1 = covar.at<double>(0,0);
		float var_2 = covar.at<double>(1,1);
		// Compute variance mean
		float var_mean = (var_1 + var_2)/2;
		// Compute variance penalty (to subtract from the maximum score)
		float penalty = var_mean/800.0f;
		// Compute maximum score
		float max_score = 1.0f - ((float)(num_cells-valid_cells))/((float)num_cells)*0.10;
		// Compute final score
		float score = max_score - penalty;
		// Re-compute modulus and phase
		//MotionVectorUtils::compute01From23(mv_drawn);
		// Show result
		//drawn = MotionVectorUtils::motionVectorToImage(mv_drawn, 0, true);
		//drawBlobContour(drawn, blob, CV_RGB(255,255,255));
		//imshow("kkk", drawn);
		//waitKey(0);
		// Check NaN
		if(score != score)
		{
			Log::warning() << "Got NaN while computing motion internal score for detection certainty, returning 0.5." << endl;
			return 0.5;
		}
		// Return score
		return score < 0 ? 0 : score;
	}
	catch(...)
	{
		Log::warning() << "Error while computing motion internal score for detection certainty, returning 0.5." << endl;
		return 0.5f;
	}
}
	
// Compute the motion_boundary_score value for an object
float DetectionEvaluator::computeMotionBoundaryScore(const Mat& motion_vector, const SampledContour& sampled_contour)
{
	// Read m parameter 
	int m = parameters.get<int>("m");
	// Debug frame
	//Mat drawn1 = drawBlob(blob);//frame.clone();
	//Mat drawn;
	//cvtColor(drawn1, drawn, CV_GRAY2BGR);
	// Initialize score
	float result = 0.0f;
	// Define normalization value
	//float normalizer = sqrt(3.0f*64.0f*64.0f);
	// Get number of points
	float k_t = sampled_contour.size();
	// Pre-compute number of elements in a neighbourhood
	int m_area = (2*m+1)*(2*m+1);
	// For each contour segment
	for(SampledContour::const_iterator it=sampled_contour.begin(); it != sampled_contour.end(); it++)
	{
		// Get the two points
		Point p1 = it->first;
		Point p2 = it->second;
		// Compute the average motion vector in the first point's neighbourhood
		Vec2f avg_1(0.0f, 0.0f);
		for(int x=p1.x-m; x<=p1.x+m; x++)
		{
			// Check frame boundaries
			if(x < 0 || x >= motion_vector.cols)
			{
				// Skip
				continue;
			}
			for(int y=p1.y-m; y<=p1.y+m; y++)
			{
				// Check frame boundaries
				if(y < 0 || y >= motion_vector.rows)
				{
					// Skip
					continue;
				}
				//drawn.at<Vec3b>(y,x) = Vec3b(0,255,255);
				avg_1[0] = avg_1[0] + ((float)motion_vector.at<Vec4d>(y,x)[2])/m_area;
				avg_1[1] = avg_1[1] + ((float)motion_vector.at<Vec4d>(y,x)[3])/m_area;
			}
		}
		// Compute the average motion vector in the second point's neighbourhood
		Vec2f avg_2(0.0f, 0.0f);
		for(int x=p2.x-m; x<=p2.x+m; x++)
		{
			// Check frame boundaries
			if(x < 0 || x >= motion_vector.cols)
			{
				// Skip
				continue;
			}
			for(int y=p2.y-m; y<=p2.y+m; y++)
			{
				// Check frame boundaries
				if(y < 0 || y >= motion_vector.rows)
				{
					// Skip
					continue;
				}
				//drawn.at<Vec3b>(y,x) = Vec3b(0,255,0);
				avg_2[0] = avg_2[0] + ((float)motion_vector.at<Vec4d>(y,x)[2])/m_area;
				avg_2[1] = avg_2[1] + ((float)motion_vector.at<Vec4d>(y,x)[3])/m_area;
			}
		}
		//line(drawn, p1, p2, CV_RGB(255,0,0));
		// Compute the contribution for this point
		float distance = sqrt((avg_1[0] - avg_2[0])*(avg_1[0] - avg_2[0])+(avg_1[1] - avg_2[1])*(avg_1[1] - avg_2[1]));
		float length_1 = sqrt(avg_1[0]*avg_1[0] + avg_1[1]*avg_1[1]);
		float length_2 = sqrt(avg_2[0]*avg_2[0] + avg_2[1]*avg_2[1]);
		float length_sum = length_1 + length_2;
		//cout << distance/length_sum << endl;
		if(length_sum != 0)
		{
			result += 1.0f/k_t*distance/length_sum;
		}
	}
	//imshow("drawn", drawn);
	//waitKey(0);
	// Return the computed value or 1 if it's larget
	return result < 0.0f ? 0.0f : result;
}

// Compute the color_boundary_score value for an object
float DetectionEvaluator::computeColorBoundaryScore(const Mat& frame, const SampledContour& sampled_contour)//, const cvb::CvBlob& blob)
{
	int m = parameters.get<int>("m");
	// Debug frame
	//Mat drawn1 = drawBlob(blob);//frame.clone();
	//Mat drawn;
	//cvtColor(drawn1, drawn, CV_GRAY2BGR);
	// Initialize score
	float result = 0.0f;
	// Define normalization value
	float normalizer = sqrt(3.0f*75.0f*75.0f);
	// Convert image to YCrCb
	Mat frame2;
	cvtColor(frame, frame2, CV_BGR2YCrCb);
	// Get number of points
	float k_t = sampled_contour.size();
	// Pre-compute number of elements in a neighbourhood
	int m_area = (2*m+1)*(2*m+1);
	// For each contour segment
	for(SampledContour::const_iterator it=sampled_contour.begin(); it != sampled_contour.end(); it++)
	{
		// Get the two points
		Point p1 = it->first;
		Point p2 = it->second;
		// Compute the average YCbCr color in the first point's neighbourhood
		Vec3f avg_1(0.0f, 0.0f, 0.0f);
		for(int x=p1.x-m; x<=p1.x+m; x++)
		{
			// Check frame boundaries
			if(x < 0 || x >= frame.cols)
			{
				// Skip
				continue;
			}
			for(int y=p1.y-m; y<=p1.y+m; y++)
			{
				// Check frame boundaries
				if(y < 0 || y >= frame.rows)
				{
					// Skip
					continue;
				}
				//drawn.at<Vec3b>(y,x) = Vec3b(0,255,255);
				avg_1[0] = avg_1[0] + ((float)frame2.at<Vec3b>(y,x)[0])/m_area;
				avg_1[1] = avg_1[1] + ((float)frame2.at<Vec3b>(y,x)[1])/m_area;
				avg_1[2] = avg_1[2] + ((float)frame2.at<Vec3b>(y,x)[2])/m_area;
			}
		}
		// Compute the average YCbCr color in the second point's neighbourhood
		Vec3f avg_2(0.0f,0.0f,0.0f);
		for(int x=p2.x-m; x<=p2.x+m; x++)
		{
			// Check frame boundaries
			if(x < 0 || x >= frame.cols)
			{
				// Skip
				continue;
			}
			for(int y=p2.y-m; y<=p2.y+m; y++)
			{
				// Check frame boundaries
				if(y < 0 || y >= frame.rows)
				{
					// Skip
					continue;
				}
				//drawn.at<Vec3b>(y,x) = Vec3b(0,255,0);
				avg_2[0] = avg_2[0] + ((float)frame2.at<Vec3b>(y,x)[0])/m_area;
				avg_2[1] = avg_2[1] + ((float)frame2.at<Vec3b>(y,x)[1])/m_area;
				avg_2[2] = avg_2[2] + ((float)frame2.at<Vec3b>(y,x)[2])/m_area;
			}
		}
		//line(drawn, p1, p2, CV_RGB(255,0,0));
		// Compute the d_cb contribution for this point
		result += 1.0f/k_t*sqrt((avg_1[0] - avg_2[0])*(avg_1[0] - avg_2[0])+(avg_1[1] - avg_2[1])*(avg_1[1] - avg_2[1])+(avg_1[2] - avg_2[2])*(avg_1[2] - avg_2[2]))/normalizer;
	}
	//imshow("drawn", drawn);
	//waitKey(0);
	// Return the computed value or 1 if it's larget
	//cout << "result: " << result << endl;
	return result > 1.0f ? 1.0f : result;
}
	
// Compute the histogram difference score for an object (using the L1 metric described by Erdem)
/*float DetectionEvaluator::computeHistogramDiffScore(const Histogram& hist_1, const Histogram& hist_2)
{
	float d = 1.0f;
	for(int k=0; k<256; k++)
	{
		d -= abs(hist_1.get(k) - hist_2.get(k))/3.5;
	}
	//d /= 2;
	return (d < 0.0f ? 0.0f : d);
}*/
