#include "blob_processing_chooser.h"
#include "blob_size_filter.h"
#include "blob_smoothing.h"
#include "blob_certainty_filter.h"
#include <algorithm>

using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> BlobProcessingChooser::valid_names;

	// Get list of names
	vector<string> BlobProcessingChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("blob_size_filter");
			valid_names.push_back("blob_smoothing");
			valid_names.push_back("blob_certainty_filter");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	BlobProcessing* BlobProcessingChooser::create(string alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid blob processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		BlobProcessing* instance;
		if(alg == "blob_size_filter")
		{
			instance = new BlobSizeFilter();
		}
		else if(alg == "blob_smoothing")
		{
			instance = new BlobSmoothing();
		}
		else if(alg == "blob_certainty_filter")
		{
			instance = new BlobCertaintyFilter();
		}
		else
		{
			stringstream error;
			error << "Invalid blob processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return instance;
	}

}
