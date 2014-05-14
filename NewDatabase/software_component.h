// A software component, as a set of algorithms
// Unlike "regular" db entities (where each instance is mapped to a table record), this class represents a set of SoftwareComponentEntryS.

#ifndef DB_SOFTWARE_COMPONENT_H
#define DB_SOFTWARE_COMPONENT_H

#include "db_entity.h"
#include <Algorithms/algorithm.h>
#include <vector>

namespace db
{

	class SoftwareComponent : public DBEntity
	{
		private:

		std::vector<alg::Algorithm*> algorithms;

		protected:

		// Setup entity fields
		void setupFields();
		
		public:

		// Constructor
		inline SoftwareComponent() : DBEntity() { setupFields(); }
		inline SoftwareComponent(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Destructor
		virtual ~SoftwareComponent() {}
		
		// Add algorithm to component
		void addAlgorithm(alg::Algorithm* algorithm);

		// Get algorithms associated to component
		inline std::vector<alg::Algorithm*> getAlgorithms() { return algorithms; }
		
		// Get values from database
		void readFromDB();

		// Lookup entity and set key(s)
		bool lookup();

		// Write values to database
		void writeToDB();
		
		// Insert new record into database
		void insertToDB();

		// Delete record from database
		void deleteFromDB();
	};

}

#endif
