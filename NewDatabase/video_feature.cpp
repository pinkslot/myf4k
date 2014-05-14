// Video feature
#include "video_feature.h"

namespace db
{

	// Setup entity fields
	void VideoFeature::setupFields()
	{
		// Set table
		setTable("video_features");
		// Add fields
		addField("video_id");
		addField("feature_id");
		addField("data");
		addField("rows");
		addField("columns");
		addField("feature_certainty");
		// Set key
		addKey("video_id", false);
		addKey("feature_id", false);
		// Set lookup fields - what is appropriate here?
		//addLookupField("video_id");
	}

}
