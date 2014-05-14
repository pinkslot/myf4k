// Represents a detection in a ground truth
// NOTE: contour coordinates are relative to bounding box
//
#ifndef DB_GT_OBJECT_H
#define DB_GT_OBJECT_H

#include "db_entity.h"
#include <cv.h>

namespace db
{

	class GtObject : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline GtObject() : DBEntity() { setupFields(); }
		inline GtObject(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Destructor
		inline ~GtObject() {}

		// Get cached field value
		template<typename T>
		T getField(std::string field_name);// { return template_ns<T>::getField(field_name); }

		// Set cached field value
		/*template<typename T>
		void setField(std::string field_name, const T& value);// { template_ns<T>::setField(field_name, value); }*/
		
	};

	// Get cached field value - generic T type, call parent function
	template<typename T>
	T GtObject::getField(std::string field_name)
	{
		return DBEntity::getField<T>(field_name);
	}

	// Set cached field value - generic T type, call parent function
	/*template<typename T>
	void GtObject::setField(std::string field_name, const T& value)
	{
		DBEntity::setField<T>(field_name, value);
	}*/

	// Get cached field value - specialized for polygon fields
	template<>
	std::vector<cv::Point> GtObject::getField<std::vector<cv::Point> >(std::string field_name);

	// Set cached field value - specialized for polygon fields
	/*template<>
	void GtObject::setField(std::string field_name, const std::vector<cv::Point>& value);*/

}

#endif
