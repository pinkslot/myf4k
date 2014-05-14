#include "perla_ground_truth.h"

namespace db
{

	// Setup entity fields
	void PerlaGroundTruth::setupFields()
	{
		// Set table
		setTable("ground_truth");
		// Add fields
		addField("id");
		addField("type");
		addField("date");
		addField("agent_id");
		addField("video_id");
		addField("best_ground_truth_id");
		addField("quality_score");
		addField("is_public");
		addField("is_deleted");
		addField("algorithm_id");
		addField("name");
		addField("derived_from");
		// Set key
		addKey("id");
		// Set auto-update fields
		addAutoUpdateField("date");
	}
		
}
