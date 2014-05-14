// Detection/tracking ground-truth

#ifndef DB_PERLA_BEST_GROUND_TRUTH_H
#define DB_PERLA_BEST_GROUND_TRUTH_H

#include "db_entity.h"

namespace db
{

	class PerlaBestGroundTruth : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline PerlaBestGroundTruth() : DBEntity() { setupFields(); }
		inline PerlaBestGroundTruth(DBConnection* conn) : DBEntity(conn) { setupFields(); }

	};

}

#endif
