#ifndef DB_RESERVED_VIDEO_H
#define DB_RESERVED_VIDEO_H

#include "db_entity.h"

namespace db
{

	class ReservedVideo : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline ReservedVideo() : DBEntity() { setupFields(); }
		inline ReservedVideo(DBConnection* conn) : DBEntity(conn) { setupFields(); }

	};

}

#endif
