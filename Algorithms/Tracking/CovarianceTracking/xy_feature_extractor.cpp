#include "xy_feature_extractor.h"

using namespace cv;

namespace alg
{

	// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
	// The input image type is supposed to be CV_8UC3.
	Mat XYFeatureExtractor::computeFeatureSet(const Mat& img, const Mat& mask)
	{
		// Compute the number of pixels to be considered
		int num_features = 0;
		for(int i=0; i<mask.rows; i++)
		{
			for(int j=0; j<mask.cols; j++)
			{
				if(mask.at<bool>(i,j)) num_features++;
			}
		}
		// Create feature matrix; each row represents a feature vector
		Mat feature_matrix(num_features, featureNumber(), CV_64F);
		// Counter for the next feature vector to write
		int feature_counter = 0;
		// Compute features: X, Y
		for(int i=0; i<img.rows; i++)
		{
			// Set the feature vector to the pixel's RGB value
			for(int j=0; j<img.cols; j++)
			{
				if(mask.at<bool>(i,j))
				{
					// Set the feature matrix values, and normalize
					feature_matrix.at<double>(feature_counter, 0) = ((double)j)/10.0;
					feature_matrix.at<double>(feature_counter, 1) = ((double)i)/10.0;
					// Increment the number of feature vectors
					feature_counter++;
				}
			}
		}
		// Return feature matrix
		return feature_matrix;
	}
}
