// Class for feature extraction from an image matrix, where each feature is the set of the pixel's color channel values.
// Naming conventions:
// - W: image width
// - H: image height
// - d: feature number

#ifndef RGB_FEATURE_EXTRACTOR_H
#define RGB_FEATURE_EXTRACTOR_H

#include "covariance_feature_extractor.h"
#include <cv.h>

using namespace cv;

namespace alg
{

	class RGBFeatureExtractor : public CovarianceFeatureExtractor
	{
		public:

		// Returns the number of features for each pixel. Subclasses should set it to the appropriate value.
		inline virtual int featureNumber() { return 3; }

		// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
		// The input image type is supposed to be CV_8UC3
		virtual Mat computeFeatureSet(const Mat&, const Mat&);
	};

}

#endif
