// Represents a video

#ifndef VIDEO_H
#define VIDEO_H

#include "db_entity.h"

namespace db
{

	class Video : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline Video() : DBEntity() { setupFields(); }
		inline Video(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
