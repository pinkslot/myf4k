// An element (instance of algorithm) included in a software_component

#ifndef DB_SOFTWARE_COMPONENT_ENTRY_H
#define DB_SOFTWARE_COMPONENT_ENTRY_H

#include "db_entity.h"
#include <algorithm.h>

namespace db
{

	class SoftwareComponentEntry : public DBEntity
	{
		private:

		protected:
		
		// Setup entity fields
		void setupFields();
		
		public:

		// Constructor
		inline SoftwareComponentEntry() : DBEntity() { setupFields(); }
		inline SoftwareComponentEntry(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Fill from alg::Algorithm
		void readAlgorithmInstance(const alg::Algorithm& algorithm);
	};

}

#endif
