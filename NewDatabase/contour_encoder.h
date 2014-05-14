// Implements a compact chain-code encoding for contours
#ifndef DB_CONTOUR_ENCODER_H
#define  DB_CONTOUR_ENCODER_H

#include <stack>
#include <vector>
#include <bitset>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv_utils.h>

namespace db
{

	class ContourEncoder
	{
		private:
		
		enum Result {OK=0, BIT_OVERFLOW=1};

		struct EncodingResult
		{
			Result result;
			std::string resultString;
		};

		// Private methods I didn't write and I don't really care about, as long as they work
		static std::vector<std::string> splitstr(std::string &s, char delim);
		static void decodePointsFromBytes(std::string ccstr, cv::Point first, std::vector<cv::Point>& cc);
		static std::string encodePointsToBytes(const std::vector<cv::Point>& pointList);
		static EncodingResult chardec2bin(unsigned char n, unsigned int padding = 0);
		static EncodingResult intdec2bin(unsigned  n, unsigned int padding = 0);
		static std::vector<cv::Point> GetPointsFromString(std::string s);
		static std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);

		public:
		
		// Convert POLYGON MySQL fields to chain-code encoding
		static std::string encode(std::string bounding_box_polygon, std::string contour_polygon);

		// Convert bounding box and contour to chain-code encoding
		static std::string encode(const std::vector<cv::Point>& bounding_box, const std::vector<cv::Point>& contour);
		static inline std::string encode(const std::vector<cv::Point>& contour) { return encode(boundingBoxFromContour(contour), contour); }

		// Convert encoded string to [bounding box and] contour
		static void decode(std::vector<cv::Point>& bb, std::vector<cv::Point>& cc, std::string data);
		static std::vector<cv::Point> decode(std::string data);

	};

}

#endif
