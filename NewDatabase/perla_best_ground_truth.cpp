#include "perla_best_ground_truth.h"

namespace db
{

	// Setup entity fields
	void PerlaBestGroundTruth::setupFields()
	{
		// Set table
		setTable("best_ground_truth");
		// Add fields
		addField("id");
		addField("type");
		addField("date");
		addField("video_id");
		// Set key
		addKey("id");
		// Set auto-update fields
		addAutoUpdateField("date");
	}
		
}
