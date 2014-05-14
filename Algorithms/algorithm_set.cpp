// Represents a set of algorithms, combined to a single algorithm entity

/*
#include "algorithm_set.h"
#include <my_exception.h>

namespace alg
{

	// Add sub-component
	void AlgorithmSet::addAlgorithm(Algorithm* algorithm)
	{
		// Check for null pointers
		if(algorithm == NULL)
		{
			throw MyException("AlgorithmSet: trying to add a NULL algorithm.");
		}
		// Add component
		components.push_back(algorithm);
	}

	// Set algorithm as source for each field
	void AlgorithmSet::setAlgorithmForInput(Algorithm* algorithm)
	{
		// Check for null pointers
		if(algorithm == NULL)
		{
			throw MyException("AlgorithmSet: trying to set source from a NULL algorithm.");
		}
		// Set source
		input_source = algorithm;
	}

	void AlgorithmSet::setAlgorithmForOutput(Algorithm* algorithm)
	{
		// Check for null pointers
		if(algorithm == NULL)
		{
			throw MyException("AlgorithmSet: trying to set source from a NULL algorithm.");
		}
		// Set source
		output_source = algorithm;
	}
		
	string AlgorithmSet::type() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		// ...
		// Build output string
		stringstream result;
		bool first_field = true;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			if(first_field)
			{
				result << (*it)->type();
				first_field = false;
			}
			else
			{
				result << "-" << (*it)->type();
			}
		}
		// Return result
		return result.str();
	}

	string AlgorithmSet::method() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		// ...
		// Build output string
		stringstream result;
		bool first_field = true;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			if(first_field)
			{
				result << (*it)->method();
				first_field = false;
			}
			else
			{
				result << "-" << (*it)->method();
			}
		}
		// Return result
		return result.str();
	}

	string AlgorithmSet::version() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		// ...
		// Build output string
		stringstream result;
		bool first_field = true;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			if(first_field)
			{
				result << (*it)->version();
				first_field = false;
			}
			else
			{
				result << "-" << (*it)->version();
			}
		}
		// Return result
		return result.str();
	}

	int AlgorithmSet::executionTime() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		// ...
		// Compute result
		int result = 0;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			result += (*it)->executionTime();
		}
		// Return result
		return result;
	}

	string AlgorithmSet::input() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		if(input_source != NULL)
		{
			return input_source->input();
		}
		// Build output string
		stringstream result;
		bool first_field = true;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			if(first_field)
			{
				result << (*it)->input();
				first_field = false;
			}
			else
			{
				result << "-" << (*it)->input();
			}
		}
		// Return result
		return result.str();
	}

	string AlgorithmSet::output() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		if(output_source != NULL)
		{
			return output_source->output();
		}
		// Build output string
		stringstream result;
		bool first_field = true;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			if(first_field)
			{
				result << (*it)->output();
				first_field = false;
			}
			else
			{
				result << "-" << (*it)->output();
			}
		}
		// Return result
		return result.str();
	}
		
	string AlgorithmSet::OS() const
	{
		// Check sub-components
		if(components.size() == 0)
		{
			throw MyException("AlgorithmSet: no algorithm added.");
		}
		// Check single source
		// ...
		// Build output string
		stringstream result;
		bool first_field = true;
		for(vector<Algorithm*>::const_iterator it = components.begin(); it != components.end(); it++)
		{
			if(first_field)
			{
				result << (*it)->OS();
				first_field = false;
			}
			else
			{
				result << "-" << (*it)->OS();
			}
		}
		// Return result
		return result.str();
	}

}*/
