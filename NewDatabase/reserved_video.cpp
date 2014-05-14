// Represents a processed video, with time and status information

#include "reserved_video.h"

using namespace std;

namespace db
{

	// Setup entity fields
	void ReservedVideo::setupFields()
	{
		// Set table
		setTable("reserved_video");
		// Add fields
		addField("id");
		addField("video_id");
		addField("reservation_time");
		// Set key
		addKey("id");
		// Set auto-update fields
		addAutoUpdateField("reservation_time");
	}
		
}
