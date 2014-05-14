// Detection/tracking ground-truth

#ifndef DB_GROUND_TRUTH_H
#define DB_GROUND_TRUTH_H

#include "db_entity.h"

namespace db
{

	class GroundTruth : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline GroundTruth() : DBEntity() { setupFields(); }
		inline GroundTruth(DBConnection* conn) : DBEntity(conn) { setupFields(); }

	};

}

#endif
