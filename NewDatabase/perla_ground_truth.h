// Detection/tracking ground-truth

#ifndef DB_PERLA_GROUND_TRUTH_H
#define DB_PERLA_GROUND_TRUTH_H

#include "db_entity.h"

namespace db
{

	class PerlaGroundTruth : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline PerlaGroundTruth() : DBEntity() { setupFields(); }
		inline PerlaGroundTruth(DBConnection* conn) : DBEntity(conn) { setupFields(); }

	};

}

#endif
