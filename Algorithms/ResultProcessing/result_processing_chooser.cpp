#include "result_processing_chooser.h"
#include <algorithm>
#include "avg_certainty.h"
#include "certainty_filter.h"
#include "lifetime_filter.h"
#include "size_filter.h"
#include "border_filter.h"
#include "avg_size_filter.h"
#include "abs_size_filter.h"
using namespace std;

namespace alg
{
	// Initialize static field
	vector<string> ResultProcessingChooser::valid_names;

	// Get list of names
	vector<string> ResultProcessingChooser::list()
	{
		// Check if list is empty
		if(valid_names.empty())
		{
			// Initialize valid names
			valid_names.push_back("avg_certainty");
			valid_names.push_back("certainty_filter");
			valid_names.push_back("lifetime_filter");
			valid_names.push_back("size_filter");
			valid_names.push_back("abs_size_filter");
			valid_names.push_back("avg_size_filter");
			valid_names.push_back("border_filter");
		}
		// Return names
		return valid_names;
	}

	// Create algorithm from name
	ResultProcessing* ResultProcessingChooser::create(string alg)
	{
		// Make sure list if filled
		list();
		// Check name exists
		if(find(valid_names.begin(), valid_names.end(), alg) == valid_names.end())
		{
			stringstream error;
			error << "Invalid result processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Instantiate algorithm
		ResultProcessing* instance;
		if(alg == "avg_certainty")
		{
			instance = new AvgCertainty();
		}
		if(alg == "certainty_filter")
		{
			instance = new CertaintyFilter();
		}
		else if(alg == "lifetime_filter")
		{
			instance = new LifetimeFilter();
		}
		else if(alg == "size_filter")
		{
			instance = new SizeFilter();
		}
		else if(alg == "abs_size_filter")
		{
			instance = new AbsSizeFilter();
		}
		else if(alg == "avg_size_filter")
		{
			instance = new AvgSizeFilter();
		}
		else if(alg == "border_filter")
		{
			instance = new BorderFilter();
		}
		else
		{
			stringstream error;
			error << "Invalid result processing algorithm '" << alg << "'.";
			throw MyException(error.str());
		}
		// Return algorithm
		return instance;
	}

}

