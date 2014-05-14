#include "blob_processing.h"
#include "opencv_utils.h"
#include <log.h>

using namespace std;
using namespace cv;
	
	
// Remove invalid blobs
void alg::BlobProcessing::removeInvalidBlobs(cvb::CvBlobs& blobs, unsigned int max_width, unsigned int max_height)
{
	// Check each blob
	for(cvb::CvBlobs::iterator it = blobs.begin(); it != blobs.end(); it++)
	{
		// Get blob
		cvb::CvBlob& blob = *(it->second);
		// Check blob
		if(!isBlobValid(blob, max_width, max_height))
		{
			// Erase blob
			blobs.erase(it);
		}
	}
}

// Get sub-filters as alg::Algorithm
vector<alg::Algorithm*> alg::BlobProcessing::getSubFiltersAsAlgs()
{
	// Initialize output vector
	vector<alg::Algorithm*> result;
	// Add filters
	for(vector<BlobProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
	{
		// Add to result
		result.push_back((alg::Algorithm*) *it);
	}
	// Return result
	return result;
}
