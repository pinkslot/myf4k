#include "postprocessing.h"

using namespace std;
	
// Get sub-filters as alg::Algorithm
vector<alg::Algorithm*> alg::PostProcessing::getSubFiltersAsAlgs()
{
	// Initialize output vector
	vector<alg::Algorithm*> result;
	// Add filters
	for(vector<PostProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
	{
		// Add to result
		result.push_back((alg::Algorithm*) *it);
	}
	// Return result
	return result;
}
