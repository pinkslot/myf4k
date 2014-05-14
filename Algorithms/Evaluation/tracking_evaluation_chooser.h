// Class for creating algorithms given their name
#ifndef ALG_TRACK_EVAL_CHOOSER_H
#define ALG_TRACK_EVAL_CHOOSER_H

#include <vector>
#include <string>
#include "evaluator.h"

namespace alg
{

	class TrackingEvaluationChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static Evaluator* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
