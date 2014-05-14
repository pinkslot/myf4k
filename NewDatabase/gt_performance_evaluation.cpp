#include "gt_performance_evaluation.h"

using namespace std;

namespace db
{

	// Setup entity fields
	void GTPerformanceEvaluation::setupFields()
	{
		// Set table
		setTable("gt_app_performance_evaluation_id");
		// Add fields
		addField("gt_id_1");
		addField("gt_id_2");
		addField("user_id");
		addField("pe_id");
		addField("time_requested");
		addField("time_completed");
		addField("is_retrieved");
		addField("video_id");
		addField("type");
		// Set key
		addKey("pe_id", true);
		// Add auto-update field
		addAutoUpdateField("time_requested");
	}
}
