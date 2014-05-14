// Class for creating tracking algorithms given their name

#ifndef ALG_DETECTION_CHOOSER_H
#define ALG_DETECTION_CHOOSER_H

#include <vector>
#include <string>
#include "detection.h"

namespace alg
{

	class DetectionChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static Detection* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
