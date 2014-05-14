// Generic feature
#include "feature.h"

namespace db
{

	// Setup entity fields
	void Feature::setupFields()
	{
		// Set table
		setTable("features");
		// Add fields
		addField("feature_id");
		addField("name");
		addField("type");
		// Set key
		addKey("feature_id");
		// Set lookup fields
		addLookupField("name");
	}

	const std::string Feature::VIDEO_DAMAGED = "video_damaged";
}
