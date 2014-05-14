// Generic feature
#ifndef FEATURE_H
#define FEATURE_H

#include "db_entity.h"

namespace db
{

	class Feature : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Feature list
		const static std::string VIDEO_DAMAGED;

		// Constructor
		inline Feature() : DBEntity() { setupFields(); }
		inline Feature(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
