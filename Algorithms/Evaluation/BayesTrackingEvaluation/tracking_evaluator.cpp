// Implementation of a tracking evaluator

#include "tracking_evaluator.h"
#include "tracking_score.h"
#include <cvblob.h>
#include <gabor_filter.h>
#include <log.h>
// Debug includes
#include <iostream>
#include <fstream>
#include <highgui.h>

using namespace cv;
using namespace std;
using namespace alg;

namespace alg
{
	
	// Constructor: no need for frame buffer
	TrackingEvaluator::TrackingEvaluator() : Evaluator(0)
	{
		// Parameters for Gabor filters
		parameters.add<int>("num_orientations", 4);
		gabor_scales.push_back(2);
		gabor_scales.push_back(4);
		gabor_scales.push_back(8);
		gabor_scales.push_back(16);
		// Initialize classifier
		//nbc.readFromFile(nbc_model_path);
		//out_file.open("train_data.txt");
	}

	// Combine parameters into a single string
	// Override Algorithm to handle scale vector
	string TrackingEvaluator::settings() const
	{
		// Initialize output string stream
		stringstream ss;
		// Call parent function
		ss << Algorithm::settings();
		// Check Gabor scale vector
		if(gabor_scales.size() > 0)
		{
			ss << ",scales=" << gabor_scales[0];
			if(gabor_scales.size() > 1)
			{
				for(vector<float>::const_iterator it = gabor_scales.begin()+1; it != gabor_scales.end(); it++)
				{
					ss << "_" << *it;
				}
			}
		}
		// Return result
		return ss.str();
	}

	// Compute single tracking score
	TrackingScore* TrackingEvaluator::computeScore(const TrackedObject& tracked_object, const Mat& frame, int frame_number, TrackingScoreHistory *history)
	{
		// Create score
		TrackingScore* score = new TrackingScore(&nbc);
		// Add score to history
		history->addScore(score, frame_number);
		// Get blob
		const cvb::CvBlob& orig_blob = tracked_object.currentRegion();
		// Random move blobs - to test certainties
		cvb::CvBlob blob;
		cvCloneBlob(orig_blob, blob);
		// Save shape ratio
		score->setShapeRatio(((float)blob.width())/((float)blob.height()));
		// Save area
		score->setArea(blob.width()*blob.height());
		// Compute object's binary mask, for the histogram
		Mat mask = drawBlob(blob, true, frame.cols, frame.rows);
		// Get grayscale frame
		Mat frame_gs;
		cvtColor(frame, frame_gs, CV_BGR2GRAY);
		// Split the color channels
		Mat* channels = new Mat[3];
		split(frame, channels);
		Mat frame_b = channels[0];
		Mat frame_g = channels[1];
		Mat frame_r = channels[2];
		delete [] channels;
		// Compute histograms
		Histogram hist_gs(frame_gs, mask);
		Histogram hist_r(frame_r, mask);
		Histogram hist_g(frame_g, mask);
		Histogram hist_b(frame_b, mask);
		// Save histogram
		score->setHistograms(hist_gs, hist_r, hist_g, hist_b);
		// Select part of frame on which to compute the texture features
		Mat copy_gs = frame_gs.clone();
		// Blacken non-mask pixels
		for(unsigned int x=blob.x; x<=blob.maxx; x++)
		{
			for(unsigned int y=blob.y; y<=blob.maxy; y++)
			{
				if(mask.at<uchar>(y,x) == 0)
				{
					copy_gs.at<uchar>(y,x) = 0;
				}
			}
		}
		// Crop image
		Rect region(blob.x, blob.y, blob.width(), blob.height());
		Mat texture_input = copy_gs(region);
		// Compute texture features for this object
		vector<float> texture_features = GaborFilter::applyFilterSet(texture_input, gabor_scales, parameters.get<int>("num_orientations"), false, 1, 0.5, 101);
		// Save texture features
		score->setTextureFeatures(texture_features);
		// Set temporal score to the number of appearances
		score->temporal_score = history->numScores();
		// Get the frame number of the previous detection of this object, if available
		int prev_frame = -1;
		TrackingScore* prev_score = NULL;
		cvb::CvBlob prev_blob;
		if(history->numScores() > 1)
		{
			// Read frame number
			prev_frame = *(tracked_object.frameList().end() - 2);
			// Get previous score
			prev_score = (TrackingScore*) history->getScore(prev_frame);
			// Get previous blob
			prev_blob = *(tracked_object.regionList().end() - 2);
			// Compute shape ratio score
			score->shape_ratio_score = computeShapeRatioScore(prev_score->getShapeRatio(), score->getShapeRatio());
			// Compute area score
			score->area_score = computeAreaScore(prev_score->getArea(), score->getArea());
			// Compute histogram difference score
			score->histogram_diff_score = computeHistogramDiffScore(score, prev_score);
			// Compute texture difference score
			score->texture_diff_score = computeTextureDiffScore(score->getTextureFeatures(), prev_score->getTextureFeatures());
			// Compute velocity for this score
			Point2f prev_position((prev_blob.maxx+prev_blob.x)/2, (prev_blob.maxy+prev_blob.y)/2);
			Point2f curr_position((blob.maxx+blob.x)/2, (blob.maxy+blob.y)/2);
			float velocity = sqrt((prev_position.x-curr_position.x)*(prev_position.x-curr_position.x) + (prev_position.y-curr_position.y)*(prev_position.y-curr_position.y));
			score->setVelocity(velocity);
			// Add velocity to the accumulated velocity for this object (will be used to compute an average according to the number of detections)
			history->accumulateVelocity(velocity);
			// Check if the previous score has its velocity set
			if(prev_score->getVelocity() > 0)
			{
				//cout << "computing ms: " << score->getVelocity() << ", " << history->getAverageVelocity() << endl;
				// Compute motion smoothness score, based on current velocity and average velocity
				score->motion_smoothness = computeMotionSmoothnessScore(score->getVelocity(), history->getAverageVelocity());
			}
			else
			{
				// Set score to 0
				score->motion_smoothness = 0.0f;
				// Mark score as not full
				score->full = false;
			}
			// Compute direction for this score
			float direction = fastAtan2(-curr_position.y + prev_position.y, curr_position.x - prev_position.x);
			score->setDirection(direction);
			// Check if the previous score has its direction set
			//cout << "direction: " << direction << ", prev_score direction: " << prev_score->getDirection() << endl;
			if(prev_score->getDirection() > 0)
			{
				//cout << "computing d : " << prev_score->getDirection() << ", " << score->getDirection() << endl;
				// Compute direction score, based on current direction and previous direction
				score->direction_score = computeDirectionScore(score->getDirection(), prev_score->getDirection());
			}
			else
			{
				// Set score to 0
				score->direction_score = 0.0f;
				// Mark score as not full
				score->full = false;
			}
			//if(score->full)// && (frame_number % 10) == 0)
			//{
			//	out_file << score->shape_ratio_score << " " << score->area_score << " " << score->histogram_diff_score << " " << score->motion_smoothness << " " << score->direction_score << " " << score->texture_diff_score << " 1" << endl;
			//	cout << score->shape_ratio_score << " " << score->area_score << " " << score->histogram_diff_score << " " << score->motion_smoothness << " " << score->direction_score << " " << score->texture_diff_score << " 1" << endl;
			//}
			//out_file << score->shape_ratio_score << " " << score->histogram_diff_score << " 1";
			//if(pd)
			//	out_file << score->value() << endl;
		}
		else
		{
			// Set score as certain
			score->certain = true;
		}
		Log::debug() << "tracking score: " << score->value() << "(" << score->shape_ratio_score << ", " << score->area_score << ", " << score->histogram_diff_score << ", " << (score->full ? score->motion_smoothness : -1) << ", " << (score->full ? score->direction_score : -1 ) << ", " << score->texture_diff_score << ")" << endl;
		// Return score
		return score;
	}

	// Perform tracking score computation for all objects in the current frame
	// TODO for some reasons, somehow prev_score->getDirection() returns 0
	void TrackingEvaluator::addFrame(const vector<const TrackedObject*>& tracked_objects, const Mat& frame, int frame_number, const Mat* next_frame)
	{
		// Process each tracked object
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
			TrackingScoreHistory* history = NULL;
			if(objects.find(tracked_object.id) == objects.end())
			{
				// Create history
				history = new TrackingScoreHistory();
				// Add history to object list
				objects[tracked_object.id] = history;
			}
			else
			{
				// Get existing score history
				history = (TrackingScoreHistory*) objects[tracked_object.id];
			}
			// Compute score
			computeScore(tracked_object, frame, frame_number, history);
		}
	}

	// Compute texture difference score
	float TrackingEvaluator::computeTextureDiffScore(vector<float> tf_1, vector<float> tf_2)
	{
		// Compute sum of squared differences
		float sum = 0.0f;
		for(unsigned int i=0; i<tf_1.size(); i++)
		{
			sum += (tf_1[i]-tf_2[i])*(tf_1[i]-tf_2[i]);
		}
		// Compute square root, i.e. distance
		float distance = sqrt(sum);
		// Compute score (check for zero)
		float score = (distance == 0 ? 1 : distance);
		// Return result
		return score;
	}

	// Compute direction score
	float TrackingEvaluator::computeDirectionScore(float d_1, float d_2)
	{
		// Compute direction difference in degrees, taking into consideration the 0/360 problem
		float diff_1 = abs(d_1 - d_2);
		float diff_2 = 360.0f - diff_1;
		float diff = (diff_1 < diff_2 ? diff_1 : diff_2);
		// Check for zero
		return (diff == 0 ? 1 : diff);
	}

	// Compute the motion smoothness score
	float TrackingEvaluator::computeMotionSmoothnessScore(float ms_1, float ms_2)
	{
		// Sort values
		float ms_min, ms_max;
		if(ms_1 < ms_2)
		{
			ms_min = ms_1;
			ms_max = ms_2;
		}
		else
		{
			ms_min = ms_2;
			ms_max = ms_1;
		}
		// Return ratio (check for zero)
		return (ms_max == 0 ? 0.1 : ms_min/ms_max);
	}

	// Compute the shape ratio score
	float TrackingEvaluator::computeShapeRatioScore(float sr_1, float sr_2)
	{
		// Sort values
		float sr_min, sr_max;
		if(sr_1 < sr_2)
		{
			sr_min = sr_1;
			sr_max = sr_2;
		}
		else
		{
			sr_min = sr_2;
			sr_max = sr_1;
		}
		// Return ratio
		return (sr_max == 0 ? 0.1 : sr_min/sr_max);
	}

	// Compute the shape ratio score
	float TrackingEvaluator::computeAreaScore(float sr_1, float sr_2)
	{
		// Sort values
		float sr_min, sr_max;
		if(sr_1 < sr_2)
		{
			sr_min = sr_1;
			sr_max = sr_2;
		}
		else
		{
			sr_min = sr_2;
			sr_max = sr_1;
		}
		// Return ratio
		return (sr_max == 0 ? 0.1 : sr_min/sr_max);
	}

	// Compute the histogram difference score for an object (using the L1 metric described by Erdem)
	float TrackingEvaluator::computeHistogramDiffScore(const TrackingScore* score_1, const TrackingScore* score_2)
	{
		// Initialize score sum, for averaging
		float score_sum = 0.0f;
		// Compute scores for gs channel
		float u = 0.0f; // union
		float i = 0.0f; // intersection
		for(int k=0; k<256; k++)
		{
			float v1 = score_1->histogram_gs.get(k);
			float v2 = score_2->histogram_gs.get(k);
			u += (v1 > v2 ? v1 : v2);
			i += (v1 < v2 ? v1 : v2);
		}
		score_sum += (u > 0 && i > 0 ? i/u : 0.5);
		// Compute score for r channel
		u = 0.0f; // union
		i = 0.0f; // intersection
		for(int k=0; k<256; k++)
		{
			float v1 = score_1->histogram_r.get(k);
			float v2 = score_2->histogram_r.get(k);
			u += (v1 > v2 ? v1 : v2);
			i += (v1 < v2 ? v1 : v2);
		}
		score_sum += (u > 0 && i > 0 ? i/u : 0.5);
		// Compute score for g channel
		u = 0.0f; // union
		i = 0.0f; // intersection
		for(int k=0; k<256; k++)
		{
			float v1 = score_1->histogram_g.get(k);
			float v2 = score_2->histogram_g.get(k);
			u += (v1 > v2 ? v1 : v2);
			i += (v1 < v2 ? v1 : v2);
		}
		score_sum += (u > 0 && i > 0 ? i/u : 0.5);
		// Compute score for b channel
		u = 0.0f; // union
		i = 0.0f; // intersection
		for(int k=0; k<256; k++)
		{
			float v1 = score_1->histogram_b.get(k);
			float v2 = score_2->histogram_b.get(k);
			u += (v1 > v2 ? v1 : v2);
			i += (v1 < v2 ? v1 : v2);
		}
		score_sum += (u > 0 && i > 0 ? i/u : 0.5);
		return score_sum;
	}

}
