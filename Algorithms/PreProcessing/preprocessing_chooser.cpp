#include "preprocessing_chooser.h"
#include "contrast_stretching.h"
#include "histeq.h"
#include <algorithm>

using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> PreProcessingChooser::valid_names;

	// Get list of names
	vector<string> PreProcessingChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("contrast_stretch");
			valid_names.push_back("histeq");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	PreProcessing* PreProcessingChooser::create(string alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid pre-processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		PreProcessing* instance;
		if(alg == "contrast_stretch")
		{
			instance = new ContrastStretching();
		}
		else if(alg == "histeq")
		{
			instance = new HistEq();
		}
		else
		{
			stringstream error;
			error << "Invalid pre-processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return instance;
	}

}

