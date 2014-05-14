// Class for a generic algorithm
// Basically, this includes a Parameters struct and provides information on the algorithm

#include "algorithm.h"
#include <sstream>

namespace alg
{

	// Combine parameters into a single string
	string Algorithm::settings() const
	{
		// Initialize output string stream
		stringstream ss;
		// Get parameters
		vector<string> param_list = parameters.list();
		// If there's at least one element, add it
		vector<string>::iterator it = param_list.begin();
		if(it != param_list.end())
		{
			// Add to string
			ss << *it << "=" << parameters.get<string>(*it);
		}
		if(param_list.size() > 1)
		{
			// Go through all other parameters
			for(it++; it != param_list.end(); it++)
			{
				// Add to string
				ss << "," << *it << "=" << parameters.get<string>(*it);
			}
		}
		// Return result
		return ss.str();
	}

}
