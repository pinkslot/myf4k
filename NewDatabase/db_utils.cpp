#include "db_utils.h"
#include <string_utils.h>

using namespace std;
using namespace cv;

namespace db
{

	// Convert MySQL POLYGON string to point vector
	vector<Point> DBUtils::polygonToPointVector(string polygon_str)
	{
		// Define result
		vector<Point> contour;
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
		return contour;
	}

}
