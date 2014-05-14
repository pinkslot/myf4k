#include "ground_truth.h"

namespace db
{

	// Setup entity fields
	void GroundTruth::setupFields()
	{
		// Set table
		setTable("ground_truth");
		// Add fields
		addField("id");
		addField("name");
		addField("type");
		addField("class");
		addField("date");
		addField("agent_id");
		addField("video_id");
		addField("best_ground_truth_id");
		addField("quality_score");
		// Set key
		addKey("id");
		// Set auto-update fields
		addAutoUpdateField("date");
	}
		
}
