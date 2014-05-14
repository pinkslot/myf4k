#include "certainty_filter.h"
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <results_utils.h>
#include <log.h>

using namespace std;
using namespace cv;

namespace alg
{

	CertaintyFilter::CertaintyFilter()
	{
		// Setup parameters
		parameters.add<float>("threshold", 0.3);
	}

	CertaintyFilter::~CertaintyFilter()
	{
	}

	pr::Results CertaintyFilter::processResults(Context& context)
	{
		// Copy parameters to local variables
		float threshold = parameters.get<float>("threshold");
		// Check threshold
		if(threshold < 0)
		{
			Log::w() << "Won't filter results by threshold " << threshold << endl;
			context.filtered_results = context.results;
		}
		else
		{
			// Compute filtered results
			context.filtered_results = pr::ResultsUtils::filterByDetectionCertainty(context.results, threshold);
		}
		// Return
		return context.filtered_results;
	}
		
}
