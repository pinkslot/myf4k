// Class for creating algorithms given their name
#ifndef ALG_BLOBPROC_CHOOSER_H
#define ALG_BLOBPROC_CHOOSER_H

#include <vector>
#include <string>
#include "blob_processing.h"

namespace alg
{

	class BlobProcessingChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static BlobProcessing* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
