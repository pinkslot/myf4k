// A generic algorithm (e.g. detection, tracking, pre-processing, evaluation)

#ifndef DB_ALGORITHM_H
#define DB_ALGORITHM_H

#include "db_entity.h"
#include <Algorithms/algorithm.h>

namespace db
{

	class Algorithm : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline Algorithm() : DBEntity() { setupFields(); }
		inline Algorithm(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Fill fields with actual algorithm
		void fillFields(const alg::Algorithm& algorithm);

		// Convert to alg::Algorithm
		alg::Algorithm* convertToAlgInstance();
	};

}

#endif
