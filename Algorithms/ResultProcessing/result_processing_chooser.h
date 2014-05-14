// Class for creating algorithms given their name
#ifndef ALG_RESULTPROC_CHOOSER_H
#define ALG_RESULTPROC_CHOOSER_H

#include <vector>
#include <string>
#include "result_processing.h"


namespace alg
{

	class ResultProcessingChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static alg::ResultProcessing* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
