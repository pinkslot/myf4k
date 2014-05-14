// Detection/tracking ground-truth

#ifndef DB_GT_KEYFRAMES_H
#define DB_GT_KEYFRAMES_H

#include "db_entity.h"

namespace db
{

	class GtKeyFrames : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline GtKeyFrames() : DBEntity() { setupFields(); }
		inline GtKeyFrames(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Destructor
		inline ~GtKeyFrames() {}

	};

}

#endif
