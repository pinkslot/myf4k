// Represents a fish

#include "fish.h"
#include "contour_encoder.h"
#include <string_utils.h>

namespace db
{

	// Setup entity fields
	void Fish::setupFields()
	{
		// Set table
		setTable("fish");
		// Add fields
		addField("fish_id");
		addField("best_video_id");
		addField("best_frame");
		//addField("best_bounding_box");
		//addField("best_contour");
		addField("best_bb_cc");
		addField("component_id");
		// Set key
		addKey("fish_id");
		// Set blob
		setBlob("best_bb_cc");
		// Add read modifiers for polygon
		//addReadModifier("best_bounding_box", "AsText(best_bounding_box) AS best_bounding_box");
		//addReadModifier("best_contour", "AsText(best_contour) AS best_contour");
		// Add write modifiers for polygon
		//addWriteModifier("best_bounding_box", "GeomFromText(", ")");
		//addWriteModifier("best_contour", "GeomFromText(", ")");
	}


	// Get cached field value - specialize for vector<Point>, for polygon fields
	template<>
	vector<Point> Fish::getField<vector<Point> >(string field_name)
	{
		// Check it's a polygon field
		if(field_name != "best_bb_cc")
		{
			// Throw exception
			throw MyException("Can't read point vector from non-polygon field");
		}
		// Decode and return
		return ContourEncoder::decode(getField<string>(field_name));
	}

	// Set cached field value - specialize for polygon fields
	/*template<>
	void Fish::setField<vector<Point> >(string field_name, const vector<Point>& value)
	{
		// Check it's a polygon field
		if(field_name != "best_contour" && field_name != "best_bounding_box")
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
