// Composition of feature extractors

#ifndef ALG_COVARIANCE_FEATURE_EXTRACTOR_SET_H
#define ALG_COVARIANCE_FEATURE_EXTRACTOR_SET_H

#include "covariance_feature_extractor.h"
#include <cv.h>
#include <vector>

namespace alg
{

	class FeatureExtractorSet : public CovarianceFeatureExtractor
	{
		private:

			std::vector<CovarianceFeatureExtractor*> extractors;

		public:

		// Constructor
		FeatureExtractorSet();

		// Destructor; frees internal objects
		virtual ~FeatureExtractorSet();

		// Add extractor
		inline void addExtractor(CovarianceFeatureExtractor* extr) { extractors.push_back(extr); }

		// Returns the number of features for each pixel, as the sum of each extractor
		virtual int featureNumber();

		// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
		// The input image type is supposed to be CV_8UC3
		virtual cv::Mat computeFeatureSet(const cv::Mat&, const cv::Mat&);
	};

}

#endif
