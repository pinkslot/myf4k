// Video feature
#ifndef VIDEO_FEATURE_H
#define VIDEO_FEATURE_H

#include "db_entity.h"

namespace db
{

	class VideoFeature : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline VideoFeature() : DBEntity() { setupFields(); }
		inline VideoFeature(DBConnection* conn) : DBEntity(conn) { setupFields(); }
	};

}

#endif
