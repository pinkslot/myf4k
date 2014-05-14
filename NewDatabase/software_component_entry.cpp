#include "software_component_entry.h"
#include <my_exception.h>

namespace db
{

	// Setup entity fields
	void SoftwareComponentEntry::setupFields()
	{
		// Set table
		setTable("software_component");
		// Add fields
		addField("id");
		addField("id_algorithm");
		addField("settings");
		// Set key
		addKey("id", false);
	}
		
	// Fill from alg::Algorithm
	void SoftwareComponentEntry::readAlgorithmInstance(const alg::Algorithm& alg_algorithm)
	{
		// Check connection
		if(connection == NULL)
		{
			throw MyException("SoftwareComponentEntry: NULL connection.");
		}
		try
		{
			// Convert alg::Algorithm to db::Algorithm
			Algorithm algorithm(connection);
			algorithm.fillFields(alg_algorithm);
			// Lookup
			if(!algorithm.lookup())
			{
				//std::stringstream error;
				//error << "Algorithm ('" << alg_algorithm.type() << "', '" << alg_algorithm.name() << "', '" << alg_algorithm.version() << "') lookup field.";
				//throw MyException(error.str());
				Log::i() << "Algorithm ('" << alg_algorithm.type() << "', '" << alg_algorithm.name() << "', '" << alg_algorithm.version() << "') lookup failed, inserting." << endl;
				algorithm.insertToDB();
			}
			// Set fields
			setField<int>("id_algorithm", algorithm.getField<int>("id"));
			setField<string>("settings", alg_algorithm.settings());
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Error while building software component entry from algorithm: " << e.what();
			throw MyException(error.str());
		}
		catch(exception& e)
		{
			std::stringstream error;
			error << "Error while building software component entry from algorithm: " << e.what();
			throw MyException(error.str());
		}
	}
		
}
