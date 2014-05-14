// Represents a ground-truth agent

#include "gt_agent.h"

namespace db
{

	// Setup entity fields
	void GTAgent::setupFields()
	{
		// Set table
		setTable("agent");
		// Add fields
		addField("id_agent");
		addField("type");
		// Set key
		addKey("id_agent");
	}

}
