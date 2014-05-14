// Represents a connection to the database, provides methods for connecting and querying
// TODO iterator
// TODO manage connections, disconnections, re-connections, double connections, this kind of stuff...
// TODO errors

#include "db_connection.h"
#include <ticpp.h>
#include <sstream>

using namespace ticpp;
using namespace sql;
using namespace std;

namespace db
{
	// Initialize cmd string
	string DBConnection::cmd = "(unknown)";
		
	// Set cmd string from ARGV
	void DBConnection::setCmd(int argc, char** argv)
	{
		stringstream cmd_ss;
		for(int i=0; i<argc; i++)
		{
			cmd_ss << argv[i] << " ";
		}
		cmd = cmd_ss.str();
	}

	// Constructors; if connection data is given, connect
	DBConnection::DBConnection()
	{
		// Set pointers to NULL
		connection = NULL;
		current_result_set = NULL;
		// Set default connection data
		host = "";
		user = "";
		password = "";
		database = "";
	}

	DBConnection::DBConnection(string h, string u, string p, string d)
	{
		// Set pointers to NULL
		connection = NULL;
		current_result_set = NULL;
		// Set connection data
		host = h;
		user = u;
		password = p;
		database = d;
		// Connect
		connect();
	}
		
	DBConnection::DBConnection(string config_file_path)
	{
		// Set pointers to NULL
		connection = NULL;
		current_result_set = NULL;
		// Open XML file
		try
		{
			// Load file
			Document doc(config_file_path);
			doc.LoadFile();
			// Get elements
			Node* mysql = doc.FirstChild("mysql");
			Element* host_elem = (Element*) mysql->FirstChild("host");
			Element* user_elem = (Element*) mysql->FirstChild("user");
			Element* password_elem = (Element*) mysql->FirstChild("password");
			Element* database_elem = (Element*) mysql->FirstChild("database");
			// Set local connection data
			host = host_elem->GetText();
			user = user_elem->GetText(false);
			password = password_elem->GetText(false);
			database = database_elem->GetText();
			// Connect
			connect();
		}
		catch(ticpp::Exception& ex)
		{
			stringstream error;
			error << "Error while reading database configuration from file: " << ex.what();
			throw MyException(error.str());
		}
	}
		
	// Destructor; frees connection
	DBConnection::~DBConnection()
	{
		// Release memory
		if(connection != NULL)			delete connection;
		if(current_result_set != NULL)		delete current_result_set;
	}

	// Set connection data
	void DBConnection::setConnectionData(string h, string u, string p, string d)
	{
		// Set connection data
		host = h;
		user = u;
		password = p;
		database = d;
	}

	// Connect to db
	void DBConnection::connect()
	{	
		// Try to connect
		try
		{
			Log::debug(0) << "DBConnection: connecting to " << user << "@" << host << " (using password: " << (password.size() > 0 ? "yes" : "no") << "), database: " << database << endl;
			// Get driver
			driver = get_driver_instance();
			connection = driver->connect(host, user, password);
			connection->setAutoCommit(true);
			connection->setSchema(database);
			// Update connection tracking data
			try
			{
				Log::debug(0) << "REPLACE INTO connection_track(connection_id, cmd, ts) SELECT connection_id(), \"" + cmd + "\", current_timestamp";		
				query("REPLACE INTO connection_track(connection_id, cmd, ts) SELECT connection_id(), \"" + cmd + "\", current_timestamp");		
			}
			catch(MyException& ex)
			{
				Log::w() << "DBConnection: could not update connection tracking data." << endl;
			}
		}
		catch(SQLException& e)
		{
			// Throw exception
			stringstream ss;
			ss << "MySQL connection error (" << e.getErrorCode() << "): " << e.what();
			throw MyException(ss.str());
		}
	}

	// Run query
	void DBConnection::query(string query)
	{
		try
		{
			// Check if we need to free the current result set
			if(current_result_set != NULL)
			{
				//Log::debug(0) << "Freeing current result set for next query." << endl;
				// Release memory
				delete current_result_set;
				current_result_set = NULL;
			}
			// Create statement
			PreparedStatement* statement = connection->prepareStatement(query);
			// Set blobs (keep list of istringstreams, to free them later)
			vector<istringstream*> isss;
			for(map<int, string>::iterator s_it = query_blobs.begin(); s_it != query_blobs.end(); ++s_it)
			{
				istringstream *tmp_blob = new istringstream(s_it->second);
				statement->setBlob(s_it->first, tmp_blob);
				isss.push_back(tmp_blob);
			}
			// Clear blobs
			clearBlobs();
			// Execute query
			statement->execute();
			// Save current result set
			current_result_set = statement->getResultSet();
			// Delete istringstreams
			for(vector<istringstream*>::iterator it = isss.begin(); it != isss.end(); ++it)
			{
				delete *it;
			}
			// Delete statement
			delete statement;
		}
		catch(SQLException& e)
		{
			// Throw exception
			stringstream ss;
			ss << "MySQL error (" << e.getErrorCode() << "): " << e.what();
			throw MyException(ss.str());
		}
	}
		
	// Lock table
	void DBConnection::lockTables(string lock_string)
	{
		Statement* statement = connection->createStatement();
		statement->execute("LOCK TABLES " + lock_string);
		delete statement;
	}

	// Unlock table
	void DBConnection::unlockTables()
	{
		Statement* statement = connection->createStatement();
		statement->execute("UNLOCK TABLES");
		delete statement;
	}
		
	// Fetch row; returns false if there are no more rows
	bool DBConnection::fetchRow()
	{
		if(current_result_set == NULL)
		{
			throw MyException("Trying to fetch row from null result set.");
		}
		return current_result_set->next();
	}

	int DBConnection::rowsCount()
	{
		if(current_result_set == NULL)
		{
			throw MyException("Trying to get row count from null result set.");
		}
		return current_result_set->rowsCount();
	}
		
	// Get a new connection with the same parameters
	DBConnection* DBConnection::newConnection()
	{
		Log::d() << "Created new connection." << endl;
		return new DBConnection(host, user, password, database);
	}

}
