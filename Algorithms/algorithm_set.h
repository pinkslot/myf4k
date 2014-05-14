// Represents a set of algorithms, combined to a single algorithm entity

#ifndef ALG_ALGORITHM_SET_H
#define ALG_ALGORITHM_SET_H
/*
#include "algorithm.h"

namespace alg
{

	class AlgorithmSet : public Algorithm
	{
		private:

		// List of sub-components
		vector<Algorithm*> components;

		// Sources for fields (if NULL, join all sub-components)
		Algorithm* input_source;
		Algorithm* output_source;

		protected:
		
		public:

		// Add sub-component
		void addAlgorithm(Algorithm* algorithm);

		// Set algorithm as source for each field
		void setAlgorithmForInput(Algorithm* algorithm);
		void setAlgorithmForOutput(Algorithm* algorithm);


		// Constructor
		inline AlgorithmSet() :
			input_source(NULL), output_source(NULL)
		{
		}

		// Information on the algorithm - concatenate sub-components, or use specified sources
		string name() const;
		string type() const;
		string description() const;
		string version() const;
		int executionTime() const;
		string descriptionExecutionTime() const;
		string ram() const;
		string language() const;
		string input() const;
		string output() const;
		
	};

}
*/
#endif
