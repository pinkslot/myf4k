// Represents a generic database entity, with a uniform mechanism for selection, insertion, deletion, etc
// TODO when calling insert and/or write, check if entity already exists

#include "db_entity.h"
#include <my_exception.h>
#include <algorithm>

using namespace std;
using namespace sql;

namespace db
{

	// Add key
	void DBEntity::addKey(string key, bool auto_increment)
	{
		// Check corresponding field exists
		if(fields.find(key) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set key, set field '" << key << "' before.";
			throw MyException(error.str());
		}
		// Add key
		keys.push_back(key);
		// Check auto-increment
		if(auto_increment)
		{
			if(auto_increment_key_set)
			{
				throw MyException("An auto-increment key has already been set.");
			}
			auto_increment_key_set = true;
			auto_increment_key = key;
		}
	}

	// Set auto-increment key
	void DBEntity::setAutoIncrementKey(string key)
	{
		// Check corresponding field exists
		if(fields.find(key) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set key, set field '" << key << "' before.";
			throw MyException(error.str());
		}
		// Check if key already exists
		if(find(keys.begin(), keys.end(), key) == keys.end())
		{
			// Insert key
			keys.push_back(key);
		}
		// Set as auto-increment key
		auto_increment_key = key;
		auto_increment_key_set = true;
	}
		
	// Set blob field
	void DBEntity::setBlob(std::string field)
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set blob , set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Check if blob already exists
		if(find(blob_fields.begin(), blob_fields.end(), field) == blob_fields.end())
		{
			// Insert blob fields
			blob_fields.push_back(field);
		}
	}
		
	// Add read modifier
	void DBEntity::addReadModifier(string field, string modifier)
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set read modifier, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Set modifier
		read_modifiers[field] = modifier;
	}

	// Add write modifier
	void DBEntity::addWriteModifier(string field, string mod_start, string mod_end)
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set read modifier, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Set modifier
		write_modifiers[field] = pair<string, string>(mod_start, mod_end);
	}
		
	// Add auto-update fields
	void DBEntity::addAutoUpdateField(string field)
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set auto-update field, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Set auto-update field
		auto_update_fields.push_back(field);
	}

	// Add lookup fields
	void DBEntity::addLookupField(string field)
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't set lookup field, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Set lookup field
		lookup_fields.push_back(field);
	}

	// Get field list
	vector<string> DBEntity::getFields() const
	{
		// Initialize list
		vector<string> field_list;
		// Go through all fields
		for(map<string,string>::const_iterator it = fields.begin(); it != fields.end(); it++)
		{
			// Add field to list
			field_list.push_back(it->first);
		}
		// Return list
		return field_list;
	}
		
	// Check if a field has been set
	bool DBEntity::fieldIsSet(string field) const
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't check if field is set, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Check field
		map<string, bool>::const_iterator f_it = fields_set.find(field);
		if(f_it == fields_set.end())
		{
			return false;
		}
		return f_it->second;
	}
		
	// Check if a field has a read modifier
	bool DBEntity::fieldHasReadModifier(string field) const
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't check read modifier, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Return result
		return read_modifiers.find(field) != read_modifiers.end();
	}

	// Get field name, including read modifier if present
	string DBEntity::getReadModifiedField(string field) const
	{
		// Check corresponding field exists
		if(fields.find(field) == fields.end())
		{
			// Throw exception
			stringstream error;
			error << "Can't check read modifier, set field '" << field << "' before.";
			throw MyException(error.str());
		}
		// Get iterator to read modifier, if it exists
		map<string, string>::const_iterator it = read_modifiers.find(field);
		// Return result
		return it != read_modifiers.end() ? it->second : "`" + field + "`";
	}
	
	// Get cached field value, with write-modifier, if present
	// Also, adds quotes to value
	std::string DBEntity::getWriteModifiedField(std::string field_name) const
	{
		// Check field exists
		std::map<std::string, std::string>::const_iterator it = fields.find(field_name);
		if(it == fields.end())
		{
			// Throw exception
			std::stringstream error;
			error << "Can't get write modifier, set field '" << field_name << "' before.";
			throw MyException(error.str());
		}
		// Read current value
		std::string str_value = "\"" + it->second + "\"";
		// Check write modifiers
		std::map<std::string, std::pair<std::string, std::string> >::const_iterator w_it = write_modifiers.find(field_name);
		if(w_it == write_modifiers.end())
		{
			// Return value itself
			return str_value;
		}
		// Return value with write modifier
		std::stringstream write_value;
		write_value << w_it->second.first << str_value << w_it->second.second;
		return write_value.str();
	}
		
	// Get values from database
	void DBEntity::readFromDB()
	{
		// Check connection
		if(connection == NULL)
		{
			throw MyException("DBEntity: NULL connection");
		}
		// Prepare query
		stringstream query;
		query << "SELECT ";
		// Add all fields, and check for select modifiers
		bool first_field = true;
		for(map<string, string>::iterator it = fields.begin(); it != fields.end(); it++)
		{
			// Get field name
			string field_name = it->first;
			// Prepare output variable
			string actual_field = "`" + field_name + "`";
			// Check if we have a read modifier
			map<string, string>::iterator sm_it = read_modifiers.find(field_name);
			if(sm_it != read_modifiers.end())
			{
				// Use the modifier instead
				actual_field = sm_it->second;
			}
			// Add to query
			if(first_field)
			{
				query << actual_field;
				first_field = false;
			}
			else
			{
				query << ", " << actual_field;
			}
		}
		// Continue query
		query << " FROM " << table;
		// Add key conditions
		if(keys.size() > 0)
		{
			// Get key iterator
			vector<string>::iterator k_it = keys.begin();
			// Read first key
			query << " WHERE `" << *k_it << "` = '" << getField<string>(*k_it) << "'";
			// Get all following key values
			for(k_it++; k_it != keys.end(); k_it++)
			{
				query << " AND `" << *k_it << "` = '" << getField<string>(*k_it) << "'";
			}
		}
		// Run query
		try
		{
			//cout << query.str() << endl;
			connection->query(query.str());
		}
		catch(SQLException& e)
		{
			stringstream error;
			error << "Query error on \"" << query.str() << "\": " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			stringstream error;
			error << "Query error on \"" << query.str() << "\".";
			throw MyException(error.str());
		}
		// Check there are results
		if(!connection->fetchRow())
		{
			stringstream error;
			error << "Query error: no rows found on \"" << query.str() << "\".";
			throw MyException(error.str());
		}
		// Read values
		for(map<string, string>::iterator it = fields.begin(); it != fields.end(); it++)
		{
			//cout << "setting " << it->first << " -> " << connection->getField<string>(it->first) << endl;
			setField(it->first, connection->getField<string>(it->first));
		}
	}

	// Lookup field and set key(s)
	bool DBEntity::lookup()
	{
		// Check connection
		if(connection == NULL)
		{
			throw MyException("DBEntity: NULL connection");
		}
		// Prepare query
		stringstream query;
		query << "SELECT ";
		// Add key fields fields, and check for select modifiers
		bool first_field = true;
		for(vector<string>::iterator key_it = keys.begin(); key_it != keys.end(); key_it++)
		{
			// Get key name
			string key = *key_it;
			// Prepare output variable
			string actual_key = "`" + key + "`";
			// Check if we have a read modifier
			map<string, string>::iterator sm_it = read_modifiers.find(key);
			if(sm_it != read_modifiers.end())
			{
				// Use the modifier instead
				actual_key = sm_it->second;
			}
			// Add to query
			if(first_field)
			{
				query << actual_key;
				first_field = false;
			}
			else
			{
				query << ", " << actual_key;
			}
		}
		// Continue query
		query << " FROM " << table << " WHERE ";
		// Blob counter
		int blob_counter = 1;
		// Add lookup fields
		first_field = true;
		vector<string> actual_lookup_fields;
		if(lookup_fields.size() > 0)
		{
			actual_lookup_fields = lookup_fields;
		}
		else
		{
			for(map<string, string>::iterator it = fields.begin(); it != fields.end(); it++)
			{
				// Get field
				string field = it->first;
				// Check it's not a key
				vector<string>::iterator search_key_it = find(keys.begin(), keys.end(), field);
				if(search_key_it != keys.end())
				{
					// Skip key
					continue;
				}
				// Add to lookup fields
				actual_lookup_fields.push_back(field);
			}
		}
		for(vector<string>::iterator lookup_it = actual_lookup_fields.begin(); lookup_it != actual_lookup_fields.end(); lookup_it++)
		{
			// Get field
			string field = *lookup_it;
			// Prepare output variable
			string actual_value;
			if(isBlobField(field))
			{
				// Set parameter
				actual_value = "?";
				// Set blob
				connection->setBlob(blob_counter++, getField<string>(field));
			}
			else
			{
				actual_value = "\"" + getField<string>(field) + "\"";
			}
			// Check if we have a write modifier
			map<string, pair<string,string> >::iterator wm_it = write_modifiers.find(field);
			if(wm_it != write_modifiers.end())
			{
				// Apply the modifier instead
				actual_value = wm_it->second.first + actual_value + wm_it->second.second;
			}
			// Add field conditions
			if(first_field)
			{
				query << "`" << field << "` = " << actual_value;
				first_field = false;
			}
			else
			{
				query << " AND `" << field << "` = " << actual_value;
			}
		}
		// Run query
		try
		{
			Log::debug() << "Lookup query: " << query.str() << endl;
			connection->query(query.str());
		}
		catch(SQLException& e)
		{
			stringstream error;
			error << "Query error on \"" << query.str() << "\": " << e.what();
			throw MyException(error.str());
		}
		catch(MyException& e)
		{
			stringstream error;
			error << "Query error on \"" << query.str() << "\": " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			stringstream error;
			error << "Query error on \"" << query.str() << "\".";
			throw MyException(error.str());
		}
		// Check there are results
		if(!connection->fetchRow())
		{
			Log::debug() << "No lookup results." << endl;
			return false;
		}
		// Read values
		Log::debug() << "Lookup succeded, found keys: ";
		for(vector<string>::iterator it = keys.begin(); it != keys.end(); it++)
		{
			//cout << "setting " << it->first << " -> " << connection->getField<string>(it->first) << endl;
			Log::debug(0) << *it << " ";
			setField(*it, connection->getField<string>(*it));
		}
		Log::debug(0) << endl;
		return true;
	}

	// Insert new record into database
	void DBEntity::insertToDB()
	{
		// Check connection
		if(connection == NULL)
		{
			throw MyException("DBEntity: NULL connection");
		}
		// Prepare query
		stringstream query;
		query << "INSERT INTO " << table << "(";
		// Go through all fields
		bool first_field = true;
		for(map<string,string>::iterator f_it = fields.begin(); f_it != fields.end(); f_it++)
		{
			// Get field
			string field = f_it->first;
			// Check if the current field is an auto_increment key
			vector<string>::iterator k_it = find(keys.begin(), keys.end(), field);
			if(k_it != keys.end())
			{
				// Check if it is an auto_increment key
				if(auto_increment_key_set && *k_it == auto_increment_key)
				{
					// Skip field
					continue;
				}
			}
			// Check if the current field is an auto-update field
			vector<string>::iterator au_it = find(auto_update_fields.begin(), auto_update_fields.end(), field);
			if(au_it != auto_update_fields.end())
			{
				// Skip field
				continue;
			}
			// Add field to query
			if(first_field)
			{
				query << "`" << field << "`";
				first_field = false;
			}
			else
			{
				query << ", `" << field << "`";
			}
		}
		// Initialize blob counter
		int blob_counter = 1;
		// Continue query
		query << ") VALUES(";
		// Go through all fields
		first_field = true;
		for(map<string,string>::iterator f_it = fields.begin(); f_it != fields.end(); f_it++)
		{
			// Get field
			string field = f_it->first;
			// Check if the current field is an auto_increment key
			vector<string>::iterator k_it = find(keys.begin(), keys.end(), field);
			if(k_it != keys.end())
			{
				// Check if it is an auto_increment key
				if(auto_increment_key_set && *k_it == auto_increment_key)
				{
					// Skip field
					continue;
				}
			}
			// Check if the current field is an auto-update field
			vector<string>::iterator au_it = find(auto_update_fields.begin(), auto_update_fields.end(), field);
			if(au_it != auto_update_fields.end())
			{
				// Skip field
				continue;
			}
			// Prepare output variable
			string actual_value;
			if(isBlobField(field))
			{
				// Set parameter
				actual_value = "?";
				// Set blob
				connection->setBlob(blob_counter++, getField<string>(field));
			}
			else
			{
				actual_value = "\"" + getField<string>(field) + "\"";
			}
			// Check if we have a write modifier
			map<string, pair<string,string> >::iterator wm_it = write_modifiers.find(field);
			if(wm_it != write_modifiers.end())
			{
				// Apply the modifier instead
				actual_value = wm_it->second.first + actual_value + wm_it->second.second;
			}
			// Add field value to query
			if(first_field)
			{
				query << actual_value;
				first_field = false;
			}
			else
			{
				query << ", " << actual_value;
			}
		}
		// Finish query
		query << ")";
		// Run query
		try
		{
			// Run insertion query
			//cout << query.str() << endl;
			connection->query(query.str());
			// Check if this entity has an auto-increment field
			if(auto_increment_key_set)
			{
				//cout << "getting key..." << endl;
				// Get inserted id
				connection->query("SELECT LAST_INSERT_ID() AS id");
				connection->fetchRow();
				//cout << connection->getField<string>("id_aaaa") << endl;
				setField<int>(auto_increment_key, connection->getField<int>("id"));
			}
		}
		catch(MyException& e)
		{
			stringstream error;
			error << "Insertion failed: " << e.what();
			throw MyException(error.str());
		}
		catch(SQLException& e)
		{
			stringstream error;
			error << "Insertion failed: " << e.what();
			throw MyException(error.str());
		}
		catch(...)
		{
			throw MyException("Insertion failed");
		}
	}

	// Write values to database, calls either insert
	void DBEntity::writeToDB()
	{
		// Check connection
		if(connection == NULL)
		{
			throw MyException("DBEntity: NULL connection");
		}
		// Initialize blob counter
		int blob_counter = 1;
		// Prepare query
		stringstream query;
		query << "UPDATE " << table << " SET ";
		// Go through all fields
		bool first_field = true;
		for(map<string,string>::iterator f_it = fields.begin(); f_it != fields.end(); f_it++)
		{
			// Get field
			string field = f_it->first;
			// Check if the current field is a key
			if(find(keys.begin(), keys.end(), field) != keys.end())
			{
				// Skip field
				continue;
			}
			// Check if the current field is an auto-update field
			vector<string>::iterator au_it = find(auto_update_fields.begin(), auto_update_fields.end(), field);
			if(au_it != auto_update_fields.end())
			{
				// Skip field
				continue;
			}
			// Check if the field has been set
			if(!fieldIsSet(field))
			{
				// Skip field
				continue;
			}
			// Prepare output variable
			string actual_value;
			if(isBlobField(field))
			{
				// Set parameter
				actual_value = "?";
				// Set blob
				connection->setBlob(blob_counter++, getField<string>(field));
			}
			else
			{
				actual_value = "\"" + getField<string>(field) + "\"";
			}
			// Check if we have a write modifier
			map<string, pair<string,string> >::iterator wm_it = write_modifiers.find(field);
			if(wm_it != write_modifiers.end())
			{
				// Apply the modifier instead
				actual_value = wm_it->second.first + actual_value + wm_it->second.second;
			}
			// Add field to query
			if(first_field)
			{
				query << "`" << field << "` = " << actual_value;
				first_field = false;
			}
			else
			{
				query << ", `" << field << "` = " << actual_value;
			}
		}
		// Go through all keys
		first_field = true;
		if(keys.size() > 0)
		{
			query << " WHERE ";
			for(vector<string>::iterator k_it = keys.begin(); k_it != keys.end(); k_it++)
			{
				// Add key constraint
				if(first_field)
				{
					query << "`" << *k_it << "` = \"" << getField<string>(*k_it) << "\"";
				}
				else
				{
					query << " AND `" << *k_it << "` = \"" << getField<string>(*k_it) << "\"";
				}
			}
		}
		// Run query
		try
		{
			connection->query(query.str());
			//cout << query.str() << endl;
		}
		catch(MyException& e)
		{
			throw MyException("Update failed: " + string(e.what()));
		}
		catch(exception& e)
		{
			throw MyException("Update failed: " + string(e.what()));
		}
		catch(...)
		{
			throw MyException("Update failed.");
		}
	}

	// Delete record from database
	void DBEntity::deleteFromDB()
	{
		// Check connection
		if(connection == NULL)
		{
			throw MyException("DBEntity: NULL connection");
		}
		// Make sure keys exist
		if(keys.size() == 0)
		{
			throw MyException("No keys, delete failed; if you want to delete all data in the table, use DBConnection::query().");
		}
		// Prepare query
		stringstream query;
		query << "DELETE FROM " << table << " WHERE ";
		// Go through all keys
		bool first_field = true;
		for(vector<string>::iterator k_it = keys.begin(); k_it != keys.end(); k_it++)
		{
			// Add key constraint
			if(first_field)
			{
				query << "`" << *k_it << "` = \"" << getField<string>(*k_it) << "\"";
			}
			else
			{
				query << " AND `" << *k_it << "` = \"" << getField<string>(*k_it) << "\"";
			}
		}
		// Run query
		try
		{
			connection->query(query.str());
			//cout << query.str() << endl;
		}
		catch(...)
		{
			throw MyException("Delete failed");
		}
	}
	
	// Insert a group of records in a single query
	void DBEntity::groupInsert(const std::vector<DBEntity*>& all_objects, DBConnection* connection, int bundle_size)
	{
		// Check number of elements
		if(all_objects.size() == 0)
		{
			return;
		}
		// Divide objects into bundles
		int num_objects = all_objects.size();
		Log::d() << "Num objects: " << num_objects << endl;
		int num_bundles = ceil(((float)num_objects)/((float)bundle_size));
		for(int bundle=0; bundle<num_bundles; bundle++)
		{
			// Compute min and max object index
			int min_o = bundle*bundle_size;
			int max_o = min(min_o + bundle_size - 1, num_objects - 1);
			Log::d() << "Writing bundle " << bundle << " (" << min_o << " to " << max_o << ")" << endl;
			// Use first item to get all entity information - this is guaranteed to work only if all items are non-customized DBEntity's
			const DBEntity& ref_entity = **(all_objects.begin());
			// Prepare query
			stringstream query;
			query << "INSERT INTO " << ref_entity.table << "(";
			// Go through all fields
			bool first_field = true;
			for(map<string,string>::const_iterator f_it = ref_entity.fields.begin(); f_it != ref_entity.fields.end(); f_it++)
			{
				// Get field
				string field = f_it->first;
				// Check if the current field is an auto_increment key
				vector<string>::const_iterator k_it = find(ref_entity.keys.begin(), ref_entity.keys.end(), field);
				if(k_it != ref_entity.keys.end())
				{
					// Check if it is an auto_increment key
					if(ref_entity.auto_increment_key_set && *k_it == ref_entity.auto_increment_key)
					{
						// Skip field
						continue;
					}
				}
				// Check if the current field is an auto-update field
				vector<string>::const_iterator au_it = find(ref_entity.auto_update_fields.begin(), ref_entity.auto_update_fields.end(), field);
				if(au_it != ref_entity.auto_update_fields.end())
				{
					// Skip field
					continue;
				}
				// Add field to query
				if(first_field)
				{
					query << "`" << field << "`";
					first_field = false;
				}
				else
				{
					query << ", `" << field << "`";
				}
			}
			// Continue query
			query << ") VALUES";
			// Initialize blob counter
			int blob_counter = 1;
			// Go through all objects
			//for(vector<DBEntity*>::const_iterator o_it = objects.begin(), o_begin = objects.begin(), o_end = objects.end(); o_it != o_end(); o_it++)
			for(int o_it=min_o; o_it <= max_o; o_it++)
			{
				// Check comma
				if(o_it != min_o)
				{
					// Add comma
					query << ", ";
				}
				// Start new value block
				query << "(";
				// Get object
				const DBEntity& object = *(all_objects[o_it]);
				// Go through all fields
				first_field = true;
				for(map<string,string>::const_iterator f_it = object.fields.begin(); f_it != object.fields.end(); f_it++)
				{
					// Get field
					string field = f_it->first;
					// Check if the current field is an auto_increment key
					vector<string>::const_iterator k_it = find(object.keys.begin(), object.keys.end(), field);
					if(k_it != object.keys.end())
					{
						// Check if it is an auto_increment key
						if(object.auto_increment_key_set && *k_it == object.auto_increment_key)
						{
							// Skip field
							continue;
						}
					}
					// Check if the current field is an auto-update field
					vector<string>::const_iterator au_it = find(object.auto_update_fields.begin(), object.auto_update_fields.end(), field);
					if(au_it != object.auto_update_fields.end())
					{
						// Skip field
						continue;
					}
					// Prepare output variable
					string actual_value;
					if(object.isBlobField(field))
					{
						// Set parameter
						actual_value = "?";
						// Set blob
						connection->setBlob(blob_counter++, object.getField<string>(field));
					}
					else
					{
						actual_value = "\"" + object.getField<string>(field) + "\"";
					}
					// Check if we have a write modifier
					map<string, pair<string,string> >::const_iterator wm_it = object.write_modifiers.find(field);
					if(wm_it != object.write_modifiers.end())
					{
						// Apply the modifier instead
						actual_value = wm_it->second.first + actual_value + wm_it->second.second;
					}
					// Add field value to query
					if(first_field)
					{
						query << actual_value;
						first_field = false;
					}
					else
					{
						query << ", " << actual_value;
					}
				}
				// Finish query
				query << ")";
			}
			// Run query
			try
			{
				// Run insertion query
				//cout << "Running query: " << endl << query.str() << endl;
				connection->query(query.str());
			}
			catch(MyException& e)
			{
				stringstream error;
				error << "Insertion failed: " << e.what();
				throw MyException(error.str());
			}
			catch(SQLException& e)
			{
				stringstream error;
				error << "Insertion failed: " << e.what();
				throw MyException(error.str());
			}
			catch(...)
			{
				throw MyException("Insertion failed");
			}
		}
	}
}
