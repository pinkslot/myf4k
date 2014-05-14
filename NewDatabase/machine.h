// Information on the machine used to process a video

#ifndef DB_MACHINE_H
#define DB_MACHINE_H

#include "db_entity.h"

namespace db
{

	class Machine : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline Machine() : DBEntity() { setupFields(); }
		inline Machine(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Get information on current system
		void readFromSystem();

	};

}

#endif
