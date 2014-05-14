// Represents a fish
// NOTE: contour coordinates are relative to bounding box
//
#ifndef FISH_H
#define FISH_H

#include "db_entity.h"
#include <cv.h>

using namespace cv;

namespace db
{

	class Fish : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline Fish() : DBEntity() { setupFields(); }
		inline Fish(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Get cached field value
		template<typename T>
		T getField(string field_name);// { return template_ns<T>::getField(field_name); }

		// Set cached field value
		template<typename T>
		void setField(string field_name, const T& value);// { template_ns<T>::setField(field_name, value); }
		
		/*// Get cached field value - specialized for polygon
		template<>
		vector<Point> getField(string field_name);
		
		// Set cached field value
		template<>
		void setField(string field_name, const vector<Point>& value);*/
	};

	// Get cached field value - generic T type, call parent function
	template<typename T>
	T Fish::getField(string field_name)
	{
		return DBEntity::getField<T>(field_name);
	}

	// Set cached field value - generic T type, call parent function
	template<typename T>
	void Fish::setField(string field_name, const T& value)
	{
		DBEntity::setField<T>(field_name, value);
	}

	// Get cached field value - specialized for polygon fields
	template<>
	vector<Point> Fish::getField<vector<Point> >(string field_name);

	// Set cached field value - specialized for polygon fields
	template<>
	void Fish::setField(string field_name, const vector<Point>& value);

}

#endif
