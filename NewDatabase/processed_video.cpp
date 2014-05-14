// Represents a processed video, with time and status information

#include "processed_video.h"

using namespace std;

namespace db
{

	// Setup entity fields
	void ProcessedVideo::setupFields()
	{
		// Set table
		setTable("processed_videos");
		// Add fields
		addField("id");
		addField("video_id");
		addField("component_id");
		addField("machine_id");
		addField("insert_time");
		addField("start_time");
		addField("end_time");
		addField("last_update");
		addField("status");
		addField("progress");
		addField("frames_with_errors");
		// Set key
		addKey("id");
		// Set auto-update fields
		addAutoUpdateField("start_time");
		addAutoUpdateField("insert_time");
		addAutoUpdateField("end_time");
		addAutoUpdateField("last_update");
		// Set lookup fields
		addLookupField("video_id");
		addLookupField("component_id");
	}
		
	// Check if we have previous results
	bool ProcessedVideo::hasPreviousResults()
	{
		try
		{
			// Prepare query on fish table
			stringstream query_fish;
			query_fish << "SELECT COUNT(*) AS c FROM fish WHERE best_video_id = '" << getField<string>("video_id") << "' AND component_id = '" << getField<int>("component_id") << "'";
			connection->query(query_fish.str());
			connection->fetchRow();
			if(connection->getField<int>("c") > 0)
			{
				return true;
			}
			// Prepare query on fish_detection table
			stringstream query_fish_detection;
			query_fish_detection << "SELECT COUNT(*) AS c FROM fish_detection WHERE video_id = '" << getField<string>("video_id") << "' AND component_id = '" << getField<int>("component_id") << "'";
			connection->query(query_fish_detection.str());
			connection->fetchRow();
			if(connection->getField<int>("c") > 0)
			{
				return true;
			}
			// No result found
			return false;
		}
		catch(MyException& e)
		{
			stringstream error;
			error << "Error while checking for previous results: " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			stringstream error;
			error << "Error while checking for previous results.";
			throw MyException(error.str());
		}
	}

	// Remove previous results
	void ProcessedVideo::clearPreviousResults()
	{
		try
		{
			// Prepare query on fish table
			stringstream query_fish;
			query_fish << "DELETE FROM fish WHERE best_video_id = '" << getField<string>("video_id") << "' AND component_id = '" << getField<int>("component_id") << "'";
			connection->query(query_fish.str());
			// Prepare query on fish_detection table
			stringstream query_fish_detection;
			query_fish_detection << "DELETE FROM fish_detection WHERE video_id = '" << getField<string>("video_id") << "' AND component_id = '" << getField<int>("component_id") << "'";
			connection->query(query_fish_detection.str());
		}
		catch(MyException& e)
		{
			stringstream error;
			error << "Error while deleting previous results: " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			stringstream error;
			error << "Error while deleting previous results.";
			throw MyException(error.str());
		}
	}

	// Set processing start time
	void ProcessedVideo::setStartTime()
	{
		if(connection == NULL)
		{
			throw MyException("ProcessedVideo: NULL connection");
		}
		try
		{
			// Prepare query
			stringstream query;
			query << "UPDATE processed_videos SET start_time = current_timestamp WHERE id = '" << getField<int>("id") << "'";
			connection->query(query.str());
		}
		catch(MyException& e)
		{
			stringstream error;
			error << "Error while setting start time: " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			stringstream error;
			error << "Error while setting start time.";
			throw MyException(error.str());
		}
	}

	// Set processing end time
	void ProcessedVideo::setEndTime()
	{
		if(connection == NULL)
		{
			throw MyException("ProcessedVideo: NULL connection");
		}
		try
		{
			// Prepare query
			stringstream query;
			query << "UPDATE processed_videos SET end_time = current_timestamp WHERE id = '" << getField<int>("id") << "'";
			connection->query(query.str());
		}
		catch(MyException& e)
		{
			stringstream error;
			error << "Error while setting end time: " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			stringstream error;
			error << "Error while setting end time.";
			throw MyException(error.str());
		}
	}

}
