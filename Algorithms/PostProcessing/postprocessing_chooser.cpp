#include "postprocessing_chooser.h"
#include <algorithm>
#include "mrf.h"
#include "erode_dilate.h"
#include "morph.h"
using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> PostProcessingChooser::valid_names;

	// Get list of names
	vector<string> PostProcessingChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("mrf");
			valid_names.push_back("erode_dilate");
			valid_names.push_back("morph");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	PostProcessing* PostProcessingChooser::create(string alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid post-processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		PostProcessing* instance;
		if(alg == "mrf")
		{
			instance = new MRF();
		}
		else if(alg == "erode_dilate")
		{
			instance = new ErodeDilate();
		}
		else if(alg == "morph")
		{
			instance = new Morph();
		}
		else
		{
			stringstream error;
			error << "Invalid post-processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return instance;
	}

}

