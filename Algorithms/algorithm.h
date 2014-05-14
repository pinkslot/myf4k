// Class for a generic algorithm
// Basically, this includes a Parameters struct and provides information on the algorithm

#ifndef ALG_ALGORITHM_H
#define ALG_ALGORITHM_H

#include "parameters.h"
#include <vector>

using namespace std;

namespace alg
{

	class Algorithm
	{
		protected:

		// Parameters
		Parameters parameters;
		
		public:

		// Get parameters
		inline Parameters& getParameters() { return parameters; }		

		// Information on the algorithm - must be filled in by derived class
		virtual string name() const = 0;				// e.g. gmm, apmm
		virtual string type() const = 0;				// e.g. detection, tracking, recognition
		virtual string description() const = 0;				// e.g. algorithm description
		virtual string version() const = 0;				// e.g. 1.2
		virtual int executionTime() const = 0;				// time (in milliseconds) to process an input unit (e.g. frame for detection algorithms, object for tracking algorithms)
		virtual string descriptionExecutionTime() const = 0;		// description of how the execution time is computer
		virtual int ram() const = 0;					// average amount of RAM used by the algorithm in MB
		virtual inline string language() const { return "C++"; }	// programming language used to implement the algorithm (in this case, C++)
		virtual string input() const = 0;				// e.g. Mat, vector<CvBlob>
		virtual string output() const = 0;				// e.g. vector<TrackedObject>
		
		virtual string settings() const;				// computed from current parameters, format k1=v1,k2=v2,...,kN=vN
	};

}

#endif
