#include "feature_extractor_set.h"

using namespace std;
using namespace cv;

namespace alg
{

	// Constructor
	FeatureExtractorSet::FeatureExtractorSet()
	{
		// Nothing to be done, really...
	}

	// Destructor; frees internal objects
	FeatureExtractorSet::~FeatureExtractorSet()
	{
		// Go through all extractors
		for(vector<CovarianceFeatureExtractor*>::iterator it = extractors.begin(); it != extractors.end(); it++)
		{
			// Free object
			delete *it;
		}
	}
	
	// Compute feature number
	int FeatureExtractorSet::featureNumber()
	{
		int feature_number = 0;
		// Go through all extractors
		for(vector<CovarianceFeatureExtractor*>::iterator it = extractors.begin(); it != extractors.end(); it++)
		{
			// Add to feature number
			feature_number += (*it)->featureNumber();
		}
		// Return result
		return feature_number;
	}

	// Returns a (WH)xd matrix, i.e. a matrix where each row is a feature vector.
	// The input image type is supposed to be CV_8UC3.
	Mat FeatureExtractorSet::computeFeatureSet(const Mat& img, const Mat& mask)
	{
		// Compute the number of pixels to be considered
		int num_feature_vectors = 0;
		for(int i=0; i<mask.rows; i++)
		{
			for(int j=0; j<mask.cols; j++)
			{
				if(mask.at<bool>(i,j)) num_feature_vectors++;
			}
		}
		// Create feature matrix; each row represents a feature vector
		Mat feature_matrix(num_feature_vectors, featureNumber(), CV_64F);
		// Fill matrix with values from each extractor
		int next_feature_col = 0;
		for(vector<CovarianceFeatureExtractor*>::iterator it = extractors.begin(); it != extractors.end(); it++)
		{
			// Get extractor
			CovarianceFeatureExtractor* extractor = *it;
			// Get sub feature matrix
			Mat sub_feature_matrix = extractor->computeFeatureSet(img, mask);
			// Copy to global feature matrix
			Mat dst_feature_matrix = feature_matrix(Range::all(), Range(next_feature_col, next_feature_col + sub_feature_matrix.cols));
			sub_feature_matrix.copyTo(dst_feature_matrix);
			// Increase column count
			next_feature_col += sub_feature_matrix.cols;
		}
		// Return feature matrix
		return feature_matrix;
	}

}
