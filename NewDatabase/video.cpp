// Represents a software component, used to process video data

#include "video.h"

namespace db
{

	// Setup entity fields
	void Video::setupFields()
	{
		// Set table
		setTable("video");
		// Add fields
		addField("video_id");
		addField("camera_id");
		addField("date_time");
		addField("length");
		addField("frame_rate");
		addField("frame_height");
		addField("frame_width");
		addField("frame_depth");
		addField("encoding");
		addField("video_file");
		// Set key
		addKey("video_id");
		// Set lookup fields
		addLookupField("camera_id");
		addLookupField("date_time");
		addLookupField("frame_rate");
		addLookupField("frame_height");
		addLookupField("frame_width");
	}

}
