#include "gt_performance_evaluation_entry.h"

using namespace std;

namespace db
{

	// Setup entity fields
	void GTPerformanceEvaluationEntry::setupFields()
	{
		// Set table
		setTable("gt_app_performance_evaluation_values");
		// Add fields
		addField("pe_id");
		addField("key");
		addField("value");
		// Set key
		addKey("pe_id", false);
		addKey("key", false);
	}
}
