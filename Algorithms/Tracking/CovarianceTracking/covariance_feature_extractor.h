// Abstract class for covariance feature extraction from an image matrix.
// Naming conventions:
// - W: image width
// - H: image height
// - d: feature number

#ifndef COVARIANCE_FEATURE_EXTRACTOR_H
#define COVARIANCE_FEATURE_EXTRACTOR_H

#include <cv.h>

class CovarianceFeatureExtractor
{
	public:

	// Returns the number of features for each pixel. Subclasses should set it to the appropriate value.
	virtual int featureNumber() = 0;

	// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
	// All pixels in the matrix are used to compute the features
	// The input image type is supposed to be CV_8UC3.
	virtual cv::Mat computeFeatureSet(const cv::Mat& region)
	{
		// Define mask matrix, with all ones
		cv::Mat mask = cv::Mat::ones(region.rows, region.cols, CV_8UC1);
		// Call generic computeFeatureSet() function
		return computeFeatureSet(region, mask);
	}
	
	// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
	// Only pixels set by the mask matrix are used for the computation of features
	// The input image type is supposed to be CV_8UC3.
	virtual cv::Mat computeFeatureSet(const cv::Mat&, const cv::Mat&) = 0;
};

#endif
