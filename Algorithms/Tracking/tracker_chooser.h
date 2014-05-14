// Class for creating tracking algorithms given their name

#ifndef ALG_TRACKER_CHOOSER_H
#define ALG_TRACKER_CHOOSER_H

#include <vector>
#include <string>
#include "tracker.h"

namespace alg
{

	class TrackerChooser
	{

		private:

		static std::vector<std::string> valid_names;

		public:
		
		// Create algorithm from name
		static Tracker* create(std::string name);

		// Get list of names
		static std::vector<std::string> list();

	};

}

#endif
