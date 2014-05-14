// Represents the tracking score for a certain object in a frame;
// Also contains information necessary to compute values at a later time (e.g. histogram)

#ifndef TRACKING_SCORE_H
#define TRACKING_SCORE_H

#include <vector>
#include <cv.h>
#include <score.h>
#include <histogram.h>
#include <naive_bayes_classifier.h>
#include <log.h>
#include <shell_utils.h>

namespace alg
{

	class TrackingScore : public Score
	{
		private:

		// Naive Bayes classifier, for value combination
		NaiveBayesClassifier* nbc;

		// Shape ratio for this score's frame
		float shape_ratio;

		// Area for this score's frame
		float area;
		
		// Velocity for this score's frame; -1 means unset
		float velocity;

		// Direction for this score's frame; -1 means unset
		float direction;

		// Gabor features for this score's frame
		std::vector<float> texture_features;

		public:

		// Temporal length
		float temporal_score;
		// Shape ratio between two frames
		float shape_ratio_score;
		// Area ration between two frames
		float area_score;
		// Histogram distance between two frames
		float histogram_diff_score;
		// Motion smoothness
		float motion_smoothness;
		// Direction change
		float direction_score;
		// Texture difference score
		float texture_diff_score;
		
		// Flag for setting this score to 1
		bool certain;

		// Flag to check if this score has been computed using all sub-scores
		bool full;
		
		// Histogram vectors for the frame to which this object refers
		Histogram histogram_gs;
		Histogram histogram_r;
		Histogram histogram_g;
		Histogram histogram_b;

		// Constructor; initializes everything to zero
		TrackingScore(NaiveBayesClassifier* nbc_) :	nbc(nbc_), velocity(-1), direction(-1), temporal_score(0), shape_ratio_score(0), area_score(0), histogram_diff_score(0), motion_smoothness(0), direction_score(0), texture_diff_score(),
					certain(false), full(true) {}

		// Save histogram
		inline void setHistograms(Histogram h_gs, Histogram h_r, Histogram h_g, Histogram h_b)
		{
			histogram_gs = h_gs;
			histogram_r = h_r;
			histogram_g = h_g;
			histogram_b = h_b;
		}
		
		// Save shape ratio
		inline void setShapeRatio(float sr) { shape_ratio = sr; }

		// Return shape ratio
		inline float getShapeRatio() { return shape_ratio; }
		
		// Save area
		inline void setArea(float ar) { area = ar; }

		// Return area
		inline float getArea() { return area; }
		
		// Save velocity
		inline void setVelocity(float v) { velocity = v; }

		// Return velocity
		inline float getVelocity() { return velocity; }
		
		// Save direction
		inline void setDirection(float d) { direction = d; }

		// Return direction
		inline float getDirection() { return direction; }
		
		// Save texture vector
		inline void setTextureFeatures(vector<float> tf) { texture_features = tf; }

		// Return direction
		inline vector<float> getTextureFeatures() { return texture_features; }
		
		// Compute single score
		virtual inline float value()
		{
			if(certain)
				return 1.0f;
			else
			{
				float best_prediction;
				std::map<int, float> prediction_map;
				std::vector<float> sample;
				/*cout << "sr:\t" << shape_ratio_score << endl;
				cout << "a:\t" << area_score << endl;
				cout << "hd:\t" << histogram_diff_score << endl;
				cout << "ms:\t" << motion_smoothness << endl;
				cout << "ds:\t" << direction_score << endl;
				cout << "td:\t" << texture_diff_score << endl;*/
				if(full)
				{
					sample.push_back(shape_ratio_score);
					sample.push_back(area_score);
					sample.push_back(histogram_diff_score);
					sample.push_back(motion_smoothness);
					sample.push_back(direction_score);
					sample.push_back(texture_diff_score);
					nbc->classify(sample, &best_prediction, &prediction_map);
				}
				else
				{
					sample.push_back(shape_ratio_score);
					sample.push_back(area_score);
					sample.push_back(histogram_diff_score);
					sample.push_back(texture_diff_score);
					std::vector<int> feature_select;
					feature_select.push_back(0);
					feature_select.push_back(1);
					feature_select.push_back(2);
					feature_select.push_back(5);
					nbc->classifySelectFeatures(feature_select, sample, &best_prediction, &prediction_map);
				}
				//for(map<int, float>::iterator it = prediction_map.begin(); it != prediction_map.end(); it++)
				//{
				//	cout << "class " << it->first << ": " << it->second << endl;
				//}
				float score = prediction_map[1];
				// Check for nan
				if(score != score)
				{
					Log::error() << ShellUtils::RED << "TrackingScore: resulting score is NaN, returning 0.5." << ShellUtils::NORMAL << endl;
					return 0.5;
				}
				return score;
			}
		}
		
	};

}

#endif
