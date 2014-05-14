// Class for creating algorithms given their name
#ifndef ALG_POSTPROC_CHOOSER_H
#define ALG_POSTPROC_CHOOSER_H

#include <vector>
#include <string>
#include "postprocessing.h"


namespace alg
{

	class PostProcessingChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static alg::PostProcessing* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
