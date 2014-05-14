// Class for feature extraction from an image matrix, where each feature is the pixel's hue values.

#ifndef ALG_COVARIANCE_DERIV_FEATURE_EXTRACTOR_H
#define ALG_COVARIANCE_DERIV_FEATURE_EXTRACTOR_H

#include "covariance_feature_extractor.h"
#include <cv.h>

namespace alg
{

	class DerivFeatureExtractor : public CovarianceFeatureExtractor
	{
		private:

		// Derivative orders
		int order_x;
		int order_y;

		public:

		// Constructor
		DerivFeatureExtractor(int o_x, int o_y) : order_x(o_x), order_y(o_y) {}

		// Returns the number of features for each pixel. Subclasses should set it to the appropriate value.
		inline virtual int featureNumber() { return 1; }

		// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
		// The input image type is supposed to be CV_8UC3
		virtual cv::Mat computeFeatureSet(const cv::Mat&, const cv::Mat&);
	};

}

#endif
