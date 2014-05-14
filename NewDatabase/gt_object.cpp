#include "gt_object.h"
#include "contour_encoder.h"
#include <string_utils.h>

using namespace std;
using namespace cv;

namespace db
{

	// Setup entity fields
	void GtObject::setupFields()
	{
		// Set table
		setTable("gt_object");
		// Add fields
		addField("detection_id");
		addField("object_id");
		addField("ground_truth_id");
		addField("type");
		addField("optional");
		addField("frame_number");
		addField("contour");
		addField("timestamp");
		addField("best_detection_id");
		// Set key
		addKey("ground_truth_id", false);
		addKey("detection_id", false);
		// Add auto-update field
		addAutoUpdateField("timestamp");
		// Set blob
		setBlob("contour");
	}

	// Get cached field value - specialize for vector<Point>, for polygon fields
	template<>
	vector<Point> GtObject::getField<vector<Point> >(string field_name)
	{
		// Check it's a polygon field
		if(field_name != "contour")
		{
			// Throw exception
			throw MyException("Can't read point vector from non-polygon field");
		}
		// Decode and return
		return ContourEncoder::decode(getField<string>(field_name));
		/*// Check it's a polygon field
		if(field_name != "contour" && field_name != "bounding_box")
		{
			// Throw exception
			throw MyException("Can't read point vector from non-polygon field");
		}
		// Define result
		vector<Point> contour;
		// Get polygon string, e.g. POLYGON((x y, x y, ...))
		string polygon_str = fields[field_name];
		// Remove "POLYGON(("
		polygon_str.replace(0, 9, "");
		// Remove "))"
		polygon_str.replace(polygon_str.size()-2, 2, "");
		// Split points
		vector<string> points_str = splitString(polygon_str, ',');
		// Split coordinates
		for(vector<string>::iterator it = points_str.begin(); it != points_str.end(); it++)
		{
			// Split into single coordinates
			vector<string> coords_str = splitString(*it, ' ');
			// Add points
			contour.push_back(Point(convertFromString<int>(coords_str[0]), convertFromString<int>(coords_str[1])));
		}
		// Return result
		return contour;*/
	}

	// Set cached field value - specialize for polygon fields
	/*template<>
	void GtObject::setField<vector<Point> >(string field_name, const vector<Point>& value)
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
