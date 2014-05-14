// Represents a detection in a ground truth
// NOTE: contour coordinates are relative to bounding box
//
#ifndef DB_PERLA_GT_OBJECT_H
#define DB_PERLA_GT_OBJECT_H

#include "db_entity.h"
#include <cv.h>

namespace db
{

	class PerlaGtObject : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline PerlaGtObject() : DBEntity() { setupFields(); }
		inline PerlaGtObject(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Get cached field value
		template<typename T>
		T getField(std::string field_name);// { return template_ns<T>::getField(field_name); }

		// Set cached field value
		template<typename T>
		void setField(std::string field_name, const T& value);// { template_ns<T>::setField(field_name, value); }
		
	};

	// Get cached field value - generic T type, call parent function
	template<typename T>
	T PerlaGtObject::getField(std::string field_name)
	{
		return DBEntity::getField<T>(field_name);
	}

	// Set cached field value - generic T type, call parent function
	template<typename T>
	void PerlaGtObject::setField(std::string field_name, const T& value)
	{
		DBEntity::setField<T>(field_name, value);
	}

	// Get cached field value - specialized for polygon fields
	template<>
	std::vector<cv::Point> PerlaGtObject::getField<std::vector<cv::Point> >(std::string field_name);

	// Set cached field value - specialized for polygon fields
	template<>
	void PerlaGtObject::setField(std::string field_name, const std::vector<cv::Point>& value);

}

#endif
