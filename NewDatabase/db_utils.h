// Utility functions
#ifndef DB_UTIL_H
#define DB_UTIL_H

#include <string>
#include <sstream>
#include <my_exception.h>
#include <vector>
#include <cv.h>

namespace db
{
	class DBUtils
	{
		public:

		// Convert MySQL POLYGON string to point vector
		static std::vector<cv::Point> polygonToPointVector(std::string polygon_str);
	};
}

#endif
