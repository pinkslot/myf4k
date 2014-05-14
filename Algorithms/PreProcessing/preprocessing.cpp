#include "preprocessing.h"

using namespace std;
	
// Get sub-filters as alg::Algorithm
vector<alg::Algorithm*> alg::PreProcessing::getSubFiltersAsAlgs()
{
	// Initialize output vector
	vector<alg::Algorithm*> result;
	// Add filters
	for(vector<PreProcessing*>::iterator it = sub_filters.begin(); it != sub_filters.end(); it++)
	{
		// Add to result
		result.push_back((alg::Algorithm*) *it);
	}
	// Return result
	return result;
}
