#include "avg_certainty.h"
#include <cv.h>
#include <highgui.h>
#include <cvblob.h>
#include <results_utils.h>
#include <log.h>

using namespace std;
using namespace cv;

namespace alg
{

	AvgCertainty::AvgCertainty()
	{
		// Setup parameters
		parameters.add<float>("threshold", 0.3);
	}

	AvgCertainty::~AvgCertainty()
	{
	}

	pr::Results AvgCertainty::processResults(Context& context)
	{
		// Copy parameters to local variables
		int threshold = parameters.get<float>("threshold");
		// Check threshold
		if(threshold < 0)
		{
			Log::w() << "Won't filter results by threshold " << threshold << endl;
			context.filtered_results = context.results;
		}
		else
		{
			// Compute filtered results
			context.filtered_results = pr::ResultsUtils::filterByAverageDetectionCertainty(context.results, threshold);
		}
		// Return
		return context.filtered_results;
	}
		
}
