// Class for feature extraction from an image matrix, where each feature is the coordinate pair of a pixel
// Naming conventions:
// - W: image width
// - H: image height
// - d: feature number

#ifndef ALG_COVARIANCE_XY_FEATURE_EXTRACTOR_H
#define ALG_COVARIANCE_XY_FEATURE_EXTRACTOR_H

#include "covariance_feature_extractor.h"
#include <cv.h>

namespace alg
{

	class XYFeatureExtractor : public CovarianceFeatureExtractor
	{
		public:

		// Returns the number of features for each pixel. Subclasses should set it to the appropriate value.
		inline virtual int featureNumber() { return 2; }

		// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
		// The input image type is supposed to be CV_8UC3
		virtual cv::Mat computeFeatureSet(const cv::Mat&, const cv::Mat&);
	};

}

#endif
