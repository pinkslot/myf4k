// Represents the detection score for a certain object in a frame;
// Also contains information necessary to compute values at a later time (e.g. histogram vector and motion vector)
#ifndef SVM_DETECTION_SCORE_H
#define SVM_DETECTION_SCORE_H

#include <vector>
#include <iostream>
#include <cv.h>
#include <log.h>
#include <score.h>
#include <histogram.h>
#include <shell_utils.h>
#include <svm_utils.h>

namespace alg
{

	class SVMDetectionScore : public Score
	{
		private:

		// Naive Bayes classifier, to combine scores
		const SVMUtils* svm;

		public:

		// Boundary complexity
		float bc;
		// Color contrast
		float cc;
		// Superpixel straddling
		float ss;
		// Motion boundary
		float mb;
		// Motion internal
		//float mi;
		// Color internal
		//float ci;
		
		// Cached score
		float cached_score;

		// Constructor; initializes everything to zero and sets classifier
		SVMDetectionScore(const SVMUtils* svm_) :
			svm(svm_), cached_score(-2)
		{
			bc = -1;
			cc = -1;
			ss = -1;
			mb = -1;
			//mi = -1;
			//ci = -1;
		}

		// Compute single score
		virtual float value()
		{
			// Check if cached score is set
			if(cached_score > -2)
			{
				// Return cached score
				return cached_score;
			}
			// Build selected feature vector
			vector<bool> select;
			select.push_back(bc > -1);
			select.push_back(cc > -1);
			select.push_back(ss > -1);
			select.push_back(mb > -1);
			//select.push_back(mi > -1);
			//select.push_back(ci > -1);
			// Build feature vector
			vector<float> features;
			features.push_back(bc);
			features.push_back(cc);
			features.push_back(ss);
			features.push_back(mb);
			//features.push_back(mi);
			//features.push_back(ci);
			// Predict
			cached_score = svm->classify(features, select);
			// Return
			return cached_score;
		}
		
	};

}

#endif
