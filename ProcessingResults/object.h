// Represents a detected object.
// Polygons are represented as vector<Point> and are always internally stored such to have the last point equal to the initial point, even if the provided input vectors don't have this property.

#ifndef PR_OBJECT_H
#define PR_OBJECT_H

#include <cv.h>
#include <vector>
#include <my_exception.h>
#include <string_utils.h>

namespace pr
{

	class Object
	{
		private:

		// Object data
		int id;
		int frame_number;
		//std::vector<cv::Point> bounding_box;
		std::vector<cv::Point> contour;
		float detection_certainty;
		float tracking_certainty;
		std::string type;
		std::string subtype;
		bool optional;

		public:

		// Constructors
		Object() :
			detection_certainty(-1),
			tracking_certainty(-1),
			type(""),
			subtype(""),
			optional(false)
		{
		}

		Object(const Object&);

		// Destructor
		inline virtual ~Object() {}

		// Assign method
		Object& operator=(const Object& other);

		// Accessor methods
		inline int getId() const { return id; }
		inline int getFrameNumber() const { return frame_number; }
		//inline const std::vector<cv::Point>& getBoundingBox() const { return bounding_box; }
		inline const std::vector<cv::Point>& getContour() const { return contour; }
		inline float getDetectionCertainty() const { return detection_certainty; }
		inline float getTrackingCertainty() const { return tracking_certainty; }
		inline std::string getType() const { return type; }
		inline std::string getSubType() const { return subtype; }
		inline bool getOptional() const { return optional; }

		// Setter methods
		inline void setId(int i) { id = i; }
		inline void setFrameNumber(int f) { frame_number = f; }
		//void setBoundingBox(const std::vector<cv::Point>& b);
		void setContour(const std::vector<cv::Point>& c);
		inline void setDetectionCertainty(float f) { detection_certainty = f; }
		inline void setTrackingCertainty(float f) { tracking_certainty = f; }
		inline void setType(std::string t) { type = toLower(t); }
		inline void setSubType(std::string t) { subtype = t; }
		inline void setOptional(bool o) { optional = o; }

		// Comparison method
		bool operator==(const Object& other) const;
		inline bool operator!=(const Object& other) const { return !(*this == other); }
		bool operator<(const Object& other) const;
		inline bool operator>(const Object& other) const { return !(*this < other) && !(*this == other); }
		inline bool operator<=(const Object& other) const { return *this < other || *this == other; }
		inline bool operator>=(const Object& other) const { return *this > other || *this == other; }
	};
}

#endif
