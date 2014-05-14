// Class for feature extraction from an image matrix, where each feature is the set of the pixel's color channel values.
// Naming conventions:
// - W: image width
// - H: image height
// - d: feature number

#include "deriv_feature_extractor.h"
#include <log.h>

using namespace cv;
using namespace std;

namespace alg
{

	// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
	// The input image type is supposed to be CV_8UC3.
	Mat DerivFeatureExtractor::computeFeatureSet(const Mat& img, const Mat& mask)
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
		// Compute derivative
		Mat gs;
		cvtColor(img, gs, CV_BGR2GRAY);
		Mat deriv;
		Sobel(gs, deriv, CV_64F, order_x, order_y);
		// Scale down
		deriv = deriv/50.0;
		// Compute features
		for(int i=0; i<deriv.rows; i++)
		{
			// Set the feature vector to the pixel's RGB value
			for(int j=0; j<deriv.cols; j++)
			{
				if(mask.at<bool>(i,j))
				{
					// Get the (i,j) pixel
					double pixel = deriv.at<double>(i, j);
					// Set the feature matrix values
					feature_matrix.at<double>(feature_counter, 0) = pixel;
					// Increment the number of feature vectors
					feature_counter++;
				}
			}
		}
		// Return feature matrix
		return feature_matrix;
	}

}
