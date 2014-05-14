#include "size_filter.h"
#include <cv.h>
#include <log.h>
#include <highgui.h>
#include <cvblob.h>
#include <results_utils.h>

using namespace std;
using namespace cv;

namespace alg
{

	SizeFilter::SizeFilter()
	{
		// Setup parameters
		// threshold is percentage of frame area
		parameters.add<float>("threshold", 0.005);
	}

	SizeFilter::~SizeFilter()
	{
	}

	pr::Results SizeFilter::processResults(Context& context)
	{
		// Copy parameters to local variables
		float threshold = parameters.get<float>("threshold");
		// Compute filtered results
		context.filtered_results = pr::ResultsUtils::filterBySize(context.results, threshold*context.frame.rows*context.frame.cols);
		// Return
		return context.filtered_results;
	}
		
}
