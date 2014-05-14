#include "bayes_detection_evaluator.h"
#include "bayes_detection_score.h"
#include <cvblob.h>
#include <lk_motion_vector.h>
#include <motion_vector_utils.h>
#include <time_utils.h>
#include <gabor_filter.h>
#include <log.h>

using namespace alg;
using namespace cv;
	
void BayesDetectionEvaluator::computeScoreThread(const cvb::CvBlob& blob, const Mat& frame, BayesDetectionScore& score, const Mat* next_frame)
{
	bool blob_deleted = false;
	boost::thread* segment_superpixels_thread = NULL;
	boost::thread* motion_vector_thread = NULL;
	try
	{
		// Read parameters
		int sp_k = parameters.get<int>("sp_k");
		int sp_min = parameters.get<int>("sp_min");
		int mv_window = parameters.get<int>("mv_window");
		float mv_deriv_weight = parameters.get<float>("mv_deriv_weight");
		int mv_max_level = parameters.get<int>("mv_max_level");
		// Set coordinates
		score.x = blob.centroid.x;
		score.y = blob.centroid.y;
		// Set area
		score.area = blob.area;
		// Compute color_boundary_score
		thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeColorContrastScore, this, boost::ref(frame), boost::ref(blob), score.color_contrast_score));
		// Compute edge density score
		thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeEdgeDensityScore, this, boost::ref(frame), boost::ref(blob), boost::ref(score.edge_density_score)));
		// Perform superpixels segmentation on the blob's region, plus some padding (will be used later)
		cvb::CvBlob padded_blob;
		cvCloneBlob(blob, padded_blob);
		int new_minx = padded_blob.minx;
		int new_maxx = padded_blob.maxx;
		int new_miny = padded_blob.miny;
		int new_maxy = padded_blob.maxy;
		new_minx -= 20; if(new_minx < 0)		new_minx = 0;
		new_maxx += 20; if(new_maxx >= frame.cols)	new_maxx = frame.cols - 1;
		new_miny -= 20; if(new_miny < 0)		new_miny = 0;
		new_maxy += 20; if(new_maxy >= frame.rows)	new_maxy = frame.rows - 1;
		padded_blob.minx = new_minx;
		padded_blob.maxx = new_maxx;
		padded_blob.miny = new_miny;
		padded_blob.maxy = new_maxy;
		Mat sp_seg_blob;
		//Mat sp_seg;
		int num_sp;
		Mat sub_frame = frame(Range(padded_blob.miny, padded_blob.maxy+1), Range(padded_blob.minx, padded_blob.maxx+1));
		segment_superpixels_thread = new boost::thread(boost::bind(segmentSuperpixels, _1, _2, _3, _4, _5), boost::ref(sub_frame), boost::ref(sp_seg_blob), boost::ref(num_sp), sp_k, sp_min);
		// Compute color internal_score
		thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeColorInternalScore, this, boost::ref(frame), boost::ref(blob), score.color_internal_score));
		// Compute texture score
		//score->texture_diff_score = -1;//computeTextureScore(frame_gs, blob, mask);
		//Log::debug(0) << "tds: " << score->texture_diff_score << endl;
		// Initialize motion vector
		LKMotionVector mv;
		mv.window_size = mv_window;
		mv.max_level = mv_max_level;
		Mat motion_vector;
		Rect curr_rect;
		// Flag to check whether we're computing the motion vector
		bool mv_valid = false;
		// If we have a previous frame, check the buffer contains it
		if(next_frame != NULL)
		{
			// Set flag
			mv_valid = true;
			// Set score as full
			score.full = true;
			// Compute the rectangle containing the detection
			curr_rect = Rect(blob.x, blob.y, blob.width(), blob.height());
			// Increase rectangle size to include sampled contour segments
			int incr = parameters.get<int>("theta_mb") + 5;
			curr_rect -= Point(incr, incr);
			curr_rect += Size(2*incr, 2*incr);
			// Check limits
			if(curr_rect.x < 0)					curr_rect.x = 0;
			if(curr_rect.y < 0)					curr_rect.y = 0;
			if(curr_rect.x + curr_rect.width - 1 >= frame.cols)	curr_rect.width = frame.cols - curr_rect.x;
			if(curr_rect.y + curr_rect.height - 1 >= frame.rows)	curr_rect.height = frame.rows - curr_rect.y;
			// Compute motion vector
			mv.addFrame(frame);
			motion_vector_thread = new boost::thread(boost::bind(static_cast<void (MotionVector::*)(const Mat&, const Rect&, Mat&)>(&MotionVector::addFrame), &mv, _1, _2, _3), boost::ref(*next_frame), boost::ref(curr_rect), boost::ref(motion_vector));
		}
		// Wait until segmentation completion
		segment_superpixels_thread->join();
		// Put the segmented image into a matrix with the same size as the original frame, setting to black all pixels outside the segmented region
		Mat sp_seg = Mat::zeros(frame.rows, frame.cols, CV_8UC3);
		Mat sp_seg_roi = sp_seg(Range(padded_blob.miny, padded_blob.maxy+1), Range(padded_blob.minx, padded_blob.maxx+1));
		sp_seg_blob.copyTo(sp_seg_roi);
		// Compute superpixels straddling score
		thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeSuperpixelsStraddlingScore, this, boost::ref(sp_seg), boost::ref(blob), boost::ref(score.spixels_straddling_score)));
		// Compute Gestalt-based score
		thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeGestaltScore, this, boost::ref(blob), frame.rows, frame.cols, boost::ref(score.gestalt_score)));
		// Wait until motion vector completion
		if(mv_valid && motion_vector_thread != NULL)
		{
			motion_vector_thread->join();
			// Compute motion boundary score
			thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeMotionBoundaryScore, this, boost::ref(motion_vector), boost::ref(blob), score.motion_boundary_score));
			// Compute motion internal score
			thread_pool.schedule(boost::bind(&BayesDetectionEvaluator::computeMotionInternalScore, this, boost::ref(motion_vector), boost::ref(blob), boost::ref(curr_rect), score.motion_internal_score));
		}
		// Wait until all threads return
		thread_pool.wait();
		// Check if we have to print NBC training data
		int training_class = parameters.get<int>("training_label");
		if(score.full && training_class > -1)
		{
			Log::debug(0) << "Writing Bayes detection evaluation training data" << endl;
			out_file_mutex.lock();
			if(!out_file.is_open())
				out_file.open(parameters.get<string>("training_out").c_str());
			stringstream score_line;
			// Build score string
			score_line	<< score.x << " " << score.y << " " 
					<< score.area << " " 
					<< score.color_contrast_score[0] << " " << score.color_contrast_score[1] << " " << score.color_contrast_score[2] << " " 
					<< score.edge_density_score << " " 
					<< score.spixels_straddling_score << " " 
					<< score.gestalt_score << " " 
					<< score.color_internal_score[0] << " " << score.color_internal_score[1] << " " << score.color_internal_score[2] << " " 
					<< score.motion_boundary_score[0] << " " << score.motion_boundary_score[1] << " " 
					<< score.motion_internal_score[0] << " " << score.motion_internal_score[1] << " " 
					<< training_class;
			// Write to file
			out_file << score_line.str() << endl;
			out_file_mutex.unlock();
			// Write to log
			log_mutex.lock();
			Log::d() << score_line.str() << endl;
			log_mutex.unlock();
		}
		// Print scores
		log_mutex.lock();
		Log::debug(0) << "cbs: " << score.color_contrast_score[0] << " " << score.color_contrast_score[1] << " " << score.color_contrast_score[2] << endl;
		Log::debug(0) << "eds: " << score.edge_density_score << endl;
		Log::debug(0) << "sss: " << score.spixels_straddling_score << endl;
		Log::debug(0) << "gs: " << score.gestalt_score << endl;
		if(score.full)
		{
			//cout << prev_score->motion_boundary_score.size() << endl;
			Log::debug(0) << "mbs: " << score.motion_boundary_score[0] << " " << score.motion_boundary_score[1] << endl;
			Log::debug(0) << "mis: " << score.motion_internal_score[0] << " " << score.motion_internal_score[1] << endl;
		}
		Log::d() << "detection score: " << score.value() << endl;
		log_mutex.unlock();
		// Delete blob (it was allocate by addFrame())
		delete &blob;
		blob_deleted = true;
		// Delete threads allocated here
		delete segment_superpixels_thread;
		if(motion_vector_thread != NULL)
			delete motion_vector_thread;
	}
	catch(boost::thread_resource_error& e)
	{
		Log::e() << ShellUtils::RED << "Error in certainty computation: maybe thread related: " << e.what() << endl;
		// Free resources
		if(!blob_deleted)			delete &blob;
		if(segment_superpixels_thread != NULL)	delete segment_superpixels_thread;
		if(motion_vector_thread != NULL)	delete motion_vector_thread;
	}
	catch(exception& e)
	{
		Log::e() << ShellUtils::RED << "Error in certainty computation: maybe thread related: " << e.what() << endl;
		// Free resources
		if(!blob_deleted)			delete &blob;
		if(segment_superpixels_thread != NULL)	delete segment_superpixels_thread;
		if(motion_vector_thread != NULL)	delete motion_vector_thread;
	}
	catch(...)
	{
		Log::e() << ShellUtils::RED << "Error in certainty computation: maybe thread related." << endl;
		// Free resources
		if(!blob_deleted)			delete &blob;
		if(segment_superpixels_thread != NULL)	delete segment_superpixels_thread;
		if(motion_vector_thread != NULL)	delete motion_vector_thread;
	}
}
