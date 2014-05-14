#ifndef GT_PERFORMANCE_EVALUATION_H
#define GT_PERFORMANCE_EVALUATION_H

#include "db_entity.h"

namespace db
{

	class GTPerformanceEvaluation : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline GTPerformanceEvaluation() : DBEntity() { setupFields(); }
		inline GTPerformanceEvaluation(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
