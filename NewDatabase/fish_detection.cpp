// Represents a fish detection

#include "fish_detection.h"
#include "contour_encoder.h"
#include <string_utils.h>

namespace db
{

	// Setup entity fields
	void FishDetection::setupFields()
	{
		// Set table
		setTable("fish_detection");
		// Add fields
		addField("detection_id");
		addField("fish_id");
		addField("video_id");
		addField("frame_id");
		//addField("bounding_box");
		//addField("contour");
		addField("bb_cc");
		addField("timestamp");
		addField("component_id");
		addField("detection_certainty");
		addField("tracking_certainty");
		// Set key
		addKey("detection_id");
		// Set blob
		setBlob("bb_cc");
		// Add read modifiers for polygon
		//addReadModifier("bounding_box", "AsText(bounding_box) AS bounding_box");
		//addReadModifier("contour", "AsText(contour) AS contour");
		// Add write modifiers for polygon
		//addWriteModifier("bounding_box", "GeomFromText(", ")");
		//addWriteModifier("contour", "GeomFromText(", ")");
		// Add auto-update field
		addAutoUpdateField("timestamp");
	}


	// Get cached field value - specialize for vector<Point>, for polygon fields
	template<>
	vector<Point> FishDetection::getField<vector<Point> >(string field_name)
	{
		// Check it's a polygon field
		if(field_name != "bb_cc")
		{
			// Throw exception
			throw MyException("Can't read point vector from non-polygon field");
		}
		// Decode and return
		return ContourEncoder::decode(getField<string>(field_name));
	}

	// Set cached field value - specialize for polygon fields
	/*template<>
	void FishDetection::setField<vector<Point> >(string field_name, const vector<Point>& value)
	{
		// Check it's a polygon field
		if(field_name != "contour" && field_name != "bounding_box")
		{
			// Throw exception
			throw MyException("Can't set point vector from non-polygon field");
		}
		// Initialize polygon string
		stringstream ss;
		ss << "POLYGON((";
		// Add first point
		if(value.size() > 0)
		{
			ss << value.begin()->x << " " << value.begin()->y;
		}
		// Add following points
		for(vector<Point>::const_iterator it = value.begin()+1; it != value.end(); it++)
		{
			ss << "," << it->x << " " << it->y;
		}
		// Close string
		ss << "))";
		// Set result as string
		setField<string>(field_name, ss.str());
	}*/

}
