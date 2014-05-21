// Class for utility functions for DB interaction
#ifndef DB_HELPER_H
#define DB_HELPER_H
#include "db_connection.h"
#include "contour_encoder.h"
#include <results.h>

namespace db
{

	class DBHelper
	{
		public:
		
		// Get results from fish/fish_detection tables
		static pr::Results getFDResults(DBConnection* connection, int processed_video_id);
	
		// Get results from ground_truth/gt_object tables
		static pr::Results getPerlaGTResults(DBConnection* connection, int ground_truth_id);
		static pr::Results getPerlaBestGTResults(DBConnection* connection, int ground_truth_id);
		static pr::Results getGTResults(DBConnection* connection, int ground_truth_id);

		// Save ground truth
		static int saveGTResults(DBConnection* connection, const pr::Results& results, std::string name, std::string type, std::string gt_class, int gt_agent_id, std::string gt_video_id);
	};
}
#endif