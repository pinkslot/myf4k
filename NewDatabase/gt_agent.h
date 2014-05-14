// Represents an agent

#ifndef GT_AGENT_H
#define GT_AGENT_H

#include "db_entity.h"

namespace db
{

	class GTAgent : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline GTAgent() : DBEntity() { setupFields(); }
		inline GTAgent(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
