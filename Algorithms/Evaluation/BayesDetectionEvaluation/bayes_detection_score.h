// Represents the detection score for a certain object in a frame;
// Also contains information necessary to compute values at a later time (e.g. histogram vector and motion vector)

#ifndef BAYES_DETECTION_SCORE_H
#define BAYES_DETECTION_SCORE_H

#include <vector>
#include <iostream>
#include <cv.h>
#include <log.h>
#include <score.h>
#include <histogram.h>
#include <naive_bayes_classifier.h>
#include <shell_utils.h>

namespace alg
{

	class BayesDetectionScore : public Score
	{
		private:

		// Naive Bayes classifier, to combine scores
		const NaiveBayesClassifier *nbc;

		public:

		// x-y coordinates of centroid
		int x;
		int y;
		// Area
		int area;
		// Measure of color differences on boundary (3 elements)
		float color_contrast_score[3];
		// Edge density score
		float edge_density_score;
		// Superpixel straddling score
		float spixels_straddling_score;
		// Gestalt-based score
		float gestalt_score;
		// Measure of motion vector differences on boundary (2 elements)
		float motion_boundary_score[2];
		// Measure of motion vector differences inside the object
		float motion_internal_score[2];
		// Measure of color differences inside the object
		float color_internal_score[3];
		// Measure of texture differences inside the object
		//float texture_diff_score;
		
		// Flag to check if all scores are available
		bool full;

		// Cached score
		float cached_score;

		// Constructor; initializes everything to zero and sets classifier
		BayesDetectionScore(const NaiveBayesClassifier* nbc_) :
			nbc(nbc_), full(false), cached_score(-1)
		{
			x = -1;
			y = -1;
			area = -1;
			color_contrast_score[0] = color_contrast_score[1] = color_contrast_score[2] = -1;
			edge_density_score = -1;
			spixels_straddling_score = -1;
			gestalt_score = -1;
			motion_boundary_score[0] = motion_boundary_score[1] = -1;
			motion_internal_score[0] = motion_internal_score[1] = -1;
			color_internal_score[0] = color_internal_score[1] = color_internal_score[2] = -1;
			//texture_diff_score = 0;
		}

		// Check if all values are different than -1
		bool isFull()
		{
			return	x != -1 &&
				y != -1 &&
				color_contrast_score[0] != -1 &&
				color_contrast_score[1] != -1 &&
				color_contrast_score[2] != -1 &&
				edge_density_score != -1 &&
				spixels_straddling_score != -1 &&
				gestalt_score != -1 &&
				motion_boundary_score[0] != -1 &&
				motion_boundary_score[1] != -1 &&
				motion_internal_score[0] != -1 &&
				motion_internal_score[1] != -1 &&
				color_internal_score[0] != -1 &&
				color_internal_score[1] != -1 &&
				color_internal_score[2] != -1;
		}

		// Compute single score
		virtual float value()
		{
			// Check if cached score is set
			if(cached_score >= 0)
			{
				// Return cached score
				return cached_score;
			}
			float best_prediction;
			std::map<int, float> prediction_map;
			std::vector<float> sample;
			if(full)
			{
				sample.push_back(x);
				sample.push_back(y);
				sample.push_back(area);
				sample.push_back(color_contrast_score[0]); sample.push_back(color_contrast_score[1]); sample.push_back(color_contrast_score[2]);
				sample.push_back(edge_density_score);
				sample.push_back(spixels_straddling_score);
				sample.push_back(gestalt_score);
				sample.push_back(color_internal_score[0]); sample.push_back(color_internal_score[1]); sample.push_back(color_internal_score[2]);
				sample.push_back(motion_boundary_score[0]); sample.push_back(motion_boundary_score[1]);
				sample.push_back(motion_internal_score[0]); sample.push_back(motion_internal_score[1]);
				std::vector<int> feature_select;
				feature_select.push_back(0); // x
				feature_select.push_back(1); // y
				feature_select.push_back(2); // area
				feature_select.push_back(3); // CC1
				feature_select.push_back(4); // CC2
				feature_select.push_back(5); // CC3
				feature_select.push_back(6); // ED
				feature_select.push_back(7); // SS
				feature_select.push_back(8); // GS
				feature_select.push_back(9); // CI1
				feature_select.push_back(10); // CI2
				feature_select.push_back(11); // CI3
				feature_select.push_back(12); // MB1
				feature_select.push_back(13); // MB2
				feature_select.push_back(14); // MI1
				feature_select.push_back(15); // MI2
				nbc->classifySelectFeatures(feature_select, sample, &best_prediction, &prediction_map);
			}
			else
			{
				sample.push_back(x);
				sample.push_back(y);
				sample.push_back(area);
				sample.push_back(color_contrast_score[0]); sample.push_back(color_contrast_score[1]); sample.push_back(color_contrast_score[2]);
				sample.push_back(edge_density_score);
				sample.push_back(spixels_straddling_score);
				sample.push_back(gestalt_score);
				sample.push_back(color_internal_score[0]); sample.push_back(color_internal_score[1]); sample.push_back(color_internal_score[2]);
				std::vector<int> feature_select;
				feature_select.push_back(0); // x
				feature_select.push_back(1); // y
				feature_select.push_back(2); // area
				feature_select.push_back(3); // CC1
				feature_select.push_back(4); // CC2
				feature_select.push_back(5); // CC3
				feature_select.push_back(6); // ED
				feature_select.push_back(7); // SS
				feature_select.push_back(8); // GS
				feature_select.push_back(9); // CI1
				feature_select.push_back(10); // CI2
				feature_select.push_back(11); // CI3
				nbc->classifySelectFeatures(feature_select, sample, &best_prediction, &prediction_map);
			}
			//for(map<int, float>::iterator it = prediction_map.begin(); it != prediction_map.end(); it++)
			//{
			//      cout << "class " << it->first << ": " << it->second << endl;
			//}
			float score = prediction_map[1];
			// Check for nan
			if(score != score)
			{
				Log::error() << ShellUtils::RED << "BayesDetectionScore: resulting score is NaN, returning 0.5." << ShellUtils::NORMAL << endl;
				score = 0.5;
			}
			// Set cached score
			cached_score = score;
			// Return
			return score;
		}
		
	};

}

#endif
