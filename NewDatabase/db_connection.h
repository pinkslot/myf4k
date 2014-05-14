// Represents a connection to the database, provides methods for connecting and querying
// TODO iterator

#ifndef DB_CONNECTION_H
#define DB_CONNECTION_H

#include <string_utils.h>
#include <log.h>
#include <my_exception.h>
#include <sstream>
#include <map>
// MySQL includes
#include <driver.h>
#include <connection.h>
#include <statement.h>
#include <prepared_statement.h>
#include <resultset.h>
#include <metadata.h>
#include <resultset_metadata.h>
#include <exception.h>
//#include <warning.h>
#include <mysql_connection.h>
#include <mysql_driver.h>

namespace db
{

	class DBConnection
	{
		private:

		// MySQL resources
		sql::Driver* driver;
		sql::Connection* connection;
		sql::ResultSet* current_result_set;

		// Blob support
		std::map<int, std::string> query_blobs;

		// Connection information
		std::string host;
		std::string user;
		std::string password;
		std::string database;

		public:

		// Data for connection_track
		static std::string cmd;

		// Set cmd string from ARGV
		static void setCmd(int argc, char** argv);

		// Constructor; if connection data is given, connect
		DBConnection();
		DBConnection(std::string h, std::string u, std::string p, std::string d);
		DBConnection(std::string config_file_path);

		// Destructor; frees connection
		virtual ~DBConnection();

		// Set connection data
		void setConnectionData(std::string h, std::string u, std::string p, std::string d);

		// Connect to db
		void connect();

		// Set blob value
		inline void setBlob(int index, std::string blob) { query_blobs[index] = blob; }

		// Clear blob values
		inline void clearBlobs() { query_blobs.clear(); }

		// Run query
		void query(std::string query);

		// Run query and return results as a list of entities
		template<typename T>
		std::vector<T*> queryList(const T& source, const std::vector<std::string>& output_strings = std::vector<std::string>(), const std::string& query_append = "");
		
		// Fetch row; returns false if there are no more rows
		bool fetchRow();

		// Get rows count
		int rowsCount();

		// Read data and convert them to the specified type
		template<typename T>
		T getField(std::string field);

		// Get a new connection with the same parameters
		DBConnection* newConnection();

		// Lock table
		void lockTables(std::string lock_str);

		// Unlock table
		void unlockTables();

	};

	// Read data and convert them to the specified type
	template<typename T>
	T DBConnection::getField(std::string field)
	{
		if(current_result_set == NULL)
		{
			throw MyException("DBConnection: NULL result set.");
		}
		// Read field from result set as string
		std::string value = current_result_set->getString(field);
		// Return converted value
		return convertFromString<T>(value);
	}
	
	// Run query and return results as a list of entities
	template<typename T>
	std::vector<T*> DBConnection::queryList(const T& source, const std::vector<std::string>& output_fields, const std::string& query_append)
	{
		// Build query
		std::stringstream query;
		query << "SELECT ";
		// Select field list source
		std::vector<std::string> all_fields = source.getFields();
		std::vector<std::string> fields = (output_fields.size() > 0 ? output_fields : all_fields);
		// Check fields size
		if(fields.size() == 0)
		{
			throw MyException("DBConnection::queryList(): No fields in entity.");
		}
		// Add fields with modifiers, if present
		std::vector<std::string>::iterator f_it = fields.begin();
		query << source.getReadModifiedField(*f_it);
		for(++f_it; f_it != fields.end(); ++f_it)
		{
			query << ", " << source.getReadModifiedField(*f_it);
		}
		query << " FROM " << source.getTable();
		// Check and add conditions
		std::stringstream conditions;
		bool first_condition = true;
		for(std::vector<std::string>::iterator f_it = all_fields.begin(); f_it != all_fields.end(); ++f_it)
		{
			// Check if this field has been set
			if(source.fieldIsSet(*f_it))
			{
				// Check first condition
				if(first_condition)
				{
					first_condition = false;
				}
				else
				{
					conditions << " AND ";
				}
				// Add to conditions
				conditions << *f_it << " = " << source.getWriteModifiedField(*f_it);
			}
		}
		if(conditions.str().size() > 0)
		{
			query << " WHERE " << conditions.str();
		}
		// Append extra data
		query << " " << query_append;
		//Log::debug(0) << query.str() << endl;
		// Initialize output vector
		std::vector<T*> results;
		// Run query
		this->query(query.str());
		while(fetchRow())
		{
			// Initialize new entity
			T* entity = new T(this);
			// Set fields
			for(std::vector<std::string>::iterator f_it = fields.begin(); f_it != fields.end(); ++f_it)
			{
				entity->template setField<std::string>(*f_it, getField<std::string>(*f_it));
			}
			// Add to vector
			results.push_back(entity);
		}
		// Return result vector
		return results;
	}
		

}

#endif
