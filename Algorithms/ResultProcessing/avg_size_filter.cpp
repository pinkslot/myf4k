#include "avg_size_filter.h"
#include <cv.h>
#include <log.h>
#include <highgui.h>
#include <cvblob.h>
#include <results_utils.h>

using namespace std;
using namespace cv;

namespace alg
{

	AvgSizeFilter::AvgSizeFilter()
	{
		// Setup parameters
		// threshold is percentage of frame area
		parameters.add<float>("threshold", 0.005);
	}

	AvgSizeFilter::~AvgSizeFilter()
	{
	}

	pr::Results AvgSizeFilter::processResults(Context& context)
	{
		// Copy parameters to local variables
		float threshold = parameters.get<float>("threshold");
		// Compute filtered results
		context.filtered_results = pr::ResultsUtils::filterByAverageSize(context.results, threshold*context.frame.rows*context.frame.cols);
		// Return
		return context.filtered_results;
	}
		
}
