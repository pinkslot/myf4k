#include "border_filter.h"
#include <cv.h>
#include <log.h>
#include <results_utils.h>

using namespace std;
using namespace cv;

namespace alg
{

	BorderFilter::BorderFilter()
	{
		// Setup parameters
		// No parameters
	}

	BorderFilter::~BorderFilter()
	{
	}

	pr::Results BorderFilter::processResults(Context& context)
	{
		// Compute filtered results
		context.filtered_results = pr::ResultsUtils::filterByBorder(context.results, context.frame.rows, context.frame.cols);
		// Return
		return context.filtered_results;
	}
		
}
