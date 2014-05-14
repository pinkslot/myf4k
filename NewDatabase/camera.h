// Represents a software component, used to process video data

#ifndef CAMERA_H
#define CAMERA_H

#include "db_entity.h"

namespace db
{

	class Camera : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline Camera() : DBEntity() { setupFields(); }
		inline Camera(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
