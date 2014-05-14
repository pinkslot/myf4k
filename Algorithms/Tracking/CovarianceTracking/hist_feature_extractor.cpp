#include "hist_feature_extractor.h"
#include <histogram.h>

using namespace cv;

namespace alg
{

	// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
	// The input image type is supposed to be CV_8UC3.
	Mat HistFeatureExtractor::computeFeatureSet(const Mat& img, const Mat& mask)
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
		// Convert to grayscale
		Mat img_gs;
		cvtColor(img, img_gs, CV_BGR2GRAY);
		// Compute features: histogram mean/moments
		for(int i=0; i<img_gs.rows; i++)
		{
			for(int j=0; j<img_gs.cols; j++)
			{
				if(mask.at<bool>(i,j))
				{
					// Get the 5x5 neighbourhood
					int i_min = ((i-2) < 0 ? 0 : i-2);
					int i_max = ((i+2) >= img_gs.rows? (img_gs.rows-1) : (i+2));
					int j_min = ((j-2) < 0 ? 0 : j-2);
					int j_max = ((j+2) >= img_gs.cols ? img_gs.cols : (i+2));
					// Get subimage
					Mat sub = img_gs(Range(i_min,i_max-i_min+1), Range(j_min,j_max-j_min+1));
					// Compute histogram
					Histogram hist(sub);
					// Set the feature matrix values
					feature_matrix.at<double>(feature_counter, 0) = hist.mean();
					// Increment the number of feature vectors
					feature_counter++;
				}
			}
		}
		// Return feature matrix
		return feature_matrix;
	}

}
