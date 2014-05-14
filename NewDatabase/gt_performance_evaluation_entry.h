#ifndef GT_PERFORMANCE_EVALUATION_ENTRY_H
#define GT_PERFORMANCE_EVALUATION_ENTRY_H

#include "db_entity.h"

namespace db
{

	class GTPerformanceEvaluationEntry : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline GTPerformanceEvaluationEntry() : DBEntity() { setupFields(); }
		inline GTPerformanceEvaluationEntry(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
