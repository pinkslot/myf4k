#include "lifetime_filter.h"
#include <cv.h>
#include <log.h>
#include <highgui.h>
#include <cvblob.h>
#include <results_utils.h>

using namespace std;
using namespace cv;

namespace alg
{

	LifetimeFilter::LifetimeFilter()
	{
		// Setup parameters
		parameters.add<int>("threshold", 3);
		parameters.add<int>("upper_threshold", 100);
	}

	LifetimeFilter::~LifetimeFilter()
	{
	}

	pr::Results LifetimeFilter::processResults(Context& context)
	{
		// Copy parameters to local variables
		int threshold = parameters.get<int>("threshold");
		int upper_threshold = parameters.get<int>("upper_threshold");
		// Check threshold
		if(threshold < 0)
		{
			Log::w() << "LifetimeFilter: threshold less than 0 (" << threshold << "), setting to 0" << endl;
			threshold = 0;
		}
		if(upper_threshold < 1)
		{
			Log::w() << "LifetimeFilter: upper threshold less than 1 (" << threshold << "), setting to 1" << endl;
			upper_threshold = 1;
		}
		// Compute filtered results
		context.filtered_results = pr::ResultsUtils::filterByNumAppearances(context.results, threshold, upper_threshold);
		// Return
		return context.filtered_results;
	}
		
}
