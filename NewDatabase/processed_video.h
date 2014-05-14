// Represents a processed video, with time and status information
// Status list:
// - starting
// - started
// - restarting
// - restarted
// - error
// - completed

#ifndef PROCESSED_VIDEO_H
#define PROCESSED_VIDEO_H

#include "db_entity.h"

namespace db
{

	class ProcessedVideo : public DBEntity
	{
		private:

		protected:

		// Setup entity fields
		void setupFields();

		public:

		// Constructor
		inline ProcessedVideo() : DBEntity() { setupFields(); }
		inline ProcessedVideo(DBConnection* conn) : DBEntity(conn) { setupFields(); }

		// Check if we have previous results
		bool hasPreviousResults();

		// Remove previous results
		void clearPreviousResults();

		// Set start/end time
		void setStartTime();
		void setEndTime();
	};

}

#endif
