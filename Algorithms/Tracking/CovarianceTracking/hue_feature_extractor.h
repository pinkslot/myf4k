// Class for feature extraction from an image matrix, where each feature is the pixel's hue values.

#ifndef ALG_COVARIANCE_HUE_FEATURE_EXTRACTOR_H
#define ALG_COVARIANCE_HUE_FEATURE_EXTRACTOR_H

#include "covariance_feature_extractor.h"
#include <cv.h>

namespace alg
{

	class HueFeatureExtractor : public CovarianceFeatureExtractor
	{
		public:

		// Returns the number of features for each pixel. Subclasses should set it to the appropriate value.
		inline virtual int featureNumber() { return 1; }

		// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
		// The input image type is supposed to be CV_8UC3
		virtual cv::Mat computeFeatureSet(const cv::Mat&, const cv::Mat&);
	};

}

#endif
