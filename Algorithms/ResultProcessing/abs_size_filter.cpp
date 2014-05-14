#include "abs_size_filter.h"
#include <cv.h>
#include <log.h>
#include <highgui.h>
#include <cvblob.h>
#include <results_utils.h>

using namespace std;
using namespace cv;

namespace alg
{

	AbsSizeFilter::AbsSizeFilter()
	{
		// Setup parameters
		// threshold is percentage of frame area
		parameters.add<int>("threshold", 30);
	}

	AbsSizeFilter::~AbsSizeFilter()
	{
	}

	pr::Results AbsSizeFilter::processResults(Context& context)
	{
		// Copy parameters to local variables
		int threshold = parameters.get<int>("threshold");
		// Compute filtered results
		context.filtered_results = pr::ResultsUtils::filterBySize(context.results, threshold);
		// Return
		return context.filtered_results;
	}
		
}
