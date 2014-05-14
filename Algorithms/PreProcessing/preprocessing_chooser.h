// Class for creating algorithms given their name
#ifndef ALG_PREPROC_CHOOSER_H
#define ALG_PREPROC_CHOOSER_H

#include <vector>
#include <string>
#include "preprocessing.h"

namespace alg
{

	class PreProcessingChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static PreProcessing* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
