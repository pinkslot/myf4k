// Represents a software component, used to process video data

#include "camera.h"

namespace db
{

	// Setup entity fields
	void Camera::setupFields()
	{
		// Set table
		setTable("cameras");
		// Add fields
		addField("camera_id");
		addField("video_number");
		addField("location");
		addField("camera_lens");
		addField("camera_angle");
		// Set key
		addKey("camera_id");
		// Set lookup fields
		addLookupField("location");
		addLookupField("video_number");
	}

}
