#include "detection_evaluation_chooser.h"
#include <bayes_detection_evaluator.h>
#include <svm_detection_evaluator.h>
#include <algorithm>

using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> DetectionEvaluationChooser::valid_names;

	// Get list of names
	vector<string> DetectionEvaluationChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("bayes_det_eval");
			valid_names.push_back("svm_det_eval");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	Evaluator* DetectionEvaluationChooser::create(string alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid detection evaluation algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		Evaluator* instance;
		if(alg == "bayes_det_eval")
		{
			instance = new BayesDetectionEvaluator();
		}
		else if(alg == "svm_det_eval")
		{
			instance = new SVMDetectionEvaluator();
		}
		else
		{
			stringstream error;
			error << "Invalid detection evaluation algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return instance;
	}

}
