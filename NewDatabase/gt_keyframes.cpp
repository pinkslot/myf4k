#include "gt_keyframes.h"

namespace db
{

	// Setup entity fields
	void GtKeyFrames::setupFields()
	{
		// Set table
		setTable("gt_keyframes");
		// Add fields
		addField("id");
		addField("ground_truth_id");
		addField("start");
		addField("end");
		addField("label");
		// Set key
		addKey("id");
	}
		
}
