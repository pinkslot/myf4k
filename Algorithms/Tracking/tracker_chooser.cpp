#include "tracker_chooser.h"
#include <algorithm>
#include <covariance_tracker.h>
#include <covariance_tracked_object.h>
#include <feature_extractor_set.h>
#include <rgb_feature_extractor.h>
#include <xy_feature_extractor.h>
#include <hue_feature_extractor.h>
#include <dummy_tracker.h>

using namespace std;

namespace alg
{
	// Initialize static fields
	vector<string> TrackerChooser::valid_names;

	// Get list of names
	vector<string> TrackerChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("cov");
			valid_names.push_back("dummy");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	Tracker* TrackerChooser::create(string tracking_alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), tracking_alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid detection algorithm '" << tracking_alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		if(tracking_alg == "cov")
		{
			return new CovarianceTracker();
		}
		else if(tracking_alg == "dummy")
		{
			return new DummyTracker();
		}
		else
		{
			stringstream error;
			error << "Invalid tracking algorithm '" << tracking_alg << "'.";
			throw MyException(error.str());
		}
	}

}

