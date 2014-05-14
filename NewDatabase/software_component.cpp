#include "software_component.h"
#include "software_component_entry.h"
#include "algorithm.h"
#include <cmath>

namespace db
{

	// Setup entity fields
	// Only specify table name
	void SoftwareComponent::setupFields()
	{
		// Set table
		setTable("software_component");
		// Set id field
		addField("id");
		addKey("id", false);
	}

	// Add algorithm to component
	void SoftwareComponent::addAlgorithm(alg::Algorithm* algorithm)
	{
		// Check for null values
		if(algorithm == NULL)
		{
			throw MyException("SoftwareComponent: NULL algorithm given.");
		}
		// Add to vector
		algorithms.push_back(algorithm);
	}

	// Read values from database
	void SoftwareComponent::readFromDB()
	{
		// Check connection is not null
		if(connection == NULL)
		{
			throw MyException("SoftwareComponent: NULL connection.");
		}
		try
		{
			// Clear current algorithm list
			algorithms.clear();
			// Select all component entries
			std::stringstream query;
			query << "SELECT id_algorithm, settings FROM " << table << " WHERE id = '" << getField<int>("id") << "'";
			connection->query(query.str());
			// Go through results
			while(connection->fetchRow())
			{
				// Get algorithm id
				int algorithm_id = connection->getField<int>("id_algorithm");
				// Read algorithm data
				Algorithm algorithm(connection);
				algorithm.setField<int>("id", algorithm_id);
				algorithm.readFromDB();
				// Convert to alg::Algorithm instance
				alg::Algorithm *alg_algorithm = algorithm.convertToAlgInstance();
				// Read parameters
				alg_algorithm->getParameters().readFromString(connection->getField<string>("settings"));
				// Add to list
				algorithms.push_back(alg_algorithm);
			}
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Error while reading software component: " << e.what();
			throw MyException(error.str());
		}
	}

	// Lookup entity and set key
	bool SoftwareComponent::lookup()
	{
		// Check connection is not null
		if(connection == NULL)
		{
			throw MyException("SoftwareComponent: NULL connection.");
		}
		try
		{
			// Get number of algorithms in this component
			int num_algorithms = algorithms.size();
			/* NEW VERSION */
			// Get the list of component id which have the same number of algorithms
			stringstream query;
			query << "SELECT id FROM software_component GROUP BY id HAVING COUNT(*) = " << num_algorithms;
			connection->query(query.str());
			list<int> ids;
			while(connection->fetchRow())
			{
				ids.push_back(connection->getField<int>("id"));
			}
			for(list<int>::iterator id_it = ids.begin(); id_it != ids.end(); id_it++)
			{
				// Get component id
				int component_id = *id_it;
				// Flag for errors
				bool error = false;
				// Check if this software component contains all algorithms
				for(int i=0; i<num_algorithms; i++)
				{
					// Get alg::Algorithm
					alg::Algorithm *alg_algorithm = algorithms[i];
					// Get db::Algorithm
					Algorithm db_algorithm(connection);
					db_algorithm.fillFields(*alg_algorithm);
					// If algorithm doesn't exist, add it
					if(!db_algorithm.lookup())
					{
						Log::debug(0) << "Algorithm '" << alg_algorithm->type() << "', '" << alg_algorithm->name() << "', '" << alg_algorithm->version() << "' not found, adding." << endl;
						db_algorithm.insertToDB();
					}
					// Check if this algorithm, with the right settings, is included
					query.str("");
					query << "SELECT id FROM software_component WHERE id = " << component_id << " AND id_algorithm = " << db_algorithm.getField<int>("id") << " AND settings = '" << alg_algorithm->settings() << "'";
					connection->query(query.str());
					if(!connection->fetchRow())
					{
						// Mark as error
						error = true;
						break;
					}
				}
				// Check result for this component
				if(!error)
				{
					// Set component id
					setField<int>("id", component_id);
					// Return true
					return true;
				}
			}
			// If we're here, we didn't find it
			return false;
			/* OLD VERSION
			// Check number of algorithms
			if(num_algorithms == 0)
			{
				// Won't even try...
				return false;
			}
			// Prepare string for joined table
			std::stringstream joined_table_ss;
			joined_table_ss << table << " AS sc1 ";
			// Check if we have to join
			if(num_algorithms > 1)
			{
				// Add joins
				for(int i=2; i<=num_algorithms; i++)
				{
					joined_table_ss << "JOIN " << table << " AS sc" << i << " ";
				}
				// Add join conditions
				joined_table_ss << "ON sc1.id = sc2.id ";
				for(int i=3; i<=num_algorithms; ++i)
				{
					joined_table_ss << "AND sc" << (i-1) << ".id = sc" << i << ".id "; 
				}
			}
			string joined_table = joined_table_ss.str();
			// Prepare query
			std::stringstream query;
			query << "SELECT sc1.id AS sc_id FROM " << joined_table << " ";
			// Select components with the specified number of algorithms
			if(num_algorithms == 1)
			{
				query << "WHERE ";
			}
			else
			{
				query << "AND ";
			}
			query << "sc1.id IN (SELECT sc1.id FROM " << joined_table << " GROUP BY id HAVING COUNT(*) = " << (pow(num_algorithms,num_algorithms)) << ") ";
			// Add condition on algorithms IDs and settings
			if(num_algorithms == 1)
			{
				query << "AND ";
			}
			else
			{
				query << "WHERE ";
			}
			alg::Algorithm *alg_algorithm_0 = algorithms[0];
			Algorithm db_algorithm_0(connection);
			db_algorithm_0.fillFields(*alg_algorithm_0);
			if(!db_algorithm_0.lookup())
			{
				Log::info(0) << "Algorithm '" << alg_algorithm_0->type() << "', '" << alg_algorithm_0->name() << "', '" << alg_algorithm_0->version() << "' not found, adding." << endl;
				db_algorithm_0.insertToDB();
			}
			query << "sc1.id_algorithm = '" << db_algorithm_0.getField<int>("id") << "' AND sc1.settings = '" << alg_algorithm_0->settings() << "' ";
			for(int i=2; i<=num_algorithms; ++i)
			{
				alg::Algorithm *alg_algorithm_i = algorithms[i-1];
				Algorithm db_algorithm_i(connection);
				db_algorithm_i.fillFields(*alg_algorithm_i);
				if(!db_algorithm_i.lookup())
				{
					Log::info(0) << "Algorithm '" << alg_algorithm_i->type() << "', '" << alg_algorithm_i->name() << "', '" << alg_algorithm_i->version() << "' not found, adding." << endl;
					db_algorithm_i.insertToDB();
				}
				query << "AND sc" << i << ".id_algorithm = '" << db_algorithm_i.getField<int>("id") << "' AND sc" << i << ".settings = '" << alg_algorithm_i->settings() << "' ";
			}
			// Run query
			connection->query(query.str());
			if(connection->rowsCount() == 0)
			{
				return false;
			}
			if(connection->rowsCount() > 1)
			{
				throw MyException("More than 1 software component matched.");
			}
			connection->fetchRow();
			setField<int>("id", connection->getField<int>("sc_id"));
			return true;
			// Select all software components which have the same number of algorithms
			// select sc1.id, sc1.id_algorithm as id1, sc2.id_algorithm as id2 from software_component as sc1 join software_component as sc2 on sc1.id = sc2.id;-- where sc1.id_algorithm = 4 and sc2.id_algorithm = 6;
			*/
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Error while looking up software component: " << e.what();
			throw MyException(error.str());
		}

	}

	// Write values to database
	void SoftwareComponent::writeToDB()
	{
		// Check connection is not null
		if(connection == NULL)
		{
			throw MyException("SoftwareComponent: NULL connection.");
		}
		try
		{
			// Check size
			if(algorithms.size() == 0)
			{
				throw MyException("No algorithms added.");
			}
			// Delete existing component
			deleteFromDB();
			// Insert again
			insertToDB();
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Error while writing software component: " << e.what();
			throw MyException(error.str());
		}
	}

	// Insert new record into database
	void SoftwareComponent::insertToDB()
	{
		// Check connection is not null
		if(connection == NULL)
		{
			throw MyException("SoftwareComponent: NULL connection.");
		}
		try
		{
			// Check size
			if(algorithms.size() == 0)
			{
				throw MyException("No algorithms in component.");
			}
			// Try looking this up first
			if(lookup())
			{
				throw MyException("Component already exists.");
			}
			// Find next component id
			std::stringstream query;
			query << "SELECT COUNT(*) AS count, MAX(id) AS next_id FROM " << table;
			connection->query(query.str());
			connection->fetchRow();
			int next_id = (connection->getField<int>("count") > 0 ? connection->getField<int>("next_id")+1 : 1);
			// Add single entries
			for(vector<alg::Algorithm*>::iterator it = algorithms.begin(); it != algorithms.end(); ++it)
			{
				// Get algorithm reference
				alg::Algorithm *alg_algorithm = *it;
				// Create software component entry
				SoftwareComponentEntry entry(connection);
				entry.readAlgorithmInstance(*alg_algorithm);
				// Set component ID
				entry.setField<int>("id", next_id);
				// Insert entry
				entry.insertToDB();
			}
			// Set local id
			setField<int>("id", next_id);
			
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Error while writing software component: " << e.what();
			throw MyException(error.str());
		}
	}

	// Delete record from database
	void SoftwareComponent::deleteFromDB()
	{
		// Check connection is not null
		if(connection == NULL)
		{
			throw MyException("SoftwareComponent: NULL connection.");
		}
		try
		{
			// Delete existing component
			std::stringstream query;
			query << "DELETE FROM software_component WHERE id = '" << getField<int>("id") << "'";
			connection->query(query.str());
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Error while writing software component: " << e.what();
			throw MyException(error.str());
		}
	}

}
