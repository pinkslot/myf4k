#include "tracking_evaluation_chooser.h"
#include <tracking_evaluator.h>
#include <algorithm>

using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> TrackingEvaluationChooser::valid_names;

	// Get list of names
	vector<string> TrackingEvaluationChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("bayes_track_eval");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	Evaluator* TrackingEvaluationChooser::create(string alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid tracking evaluation algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		Evaluator* instance;
		if(alg == "bayes_track_eval")
		{
			instance = new TrackingEvaluator();
		}
		else
		{
			stringstream error;
			error << "Invalid tracking evaluation algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return instance;
	}

}
