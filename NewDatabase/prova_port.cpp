// Represents a connection to the database, provides methods for connecting and querying
// TODO iterator
// TODO manage connections, disconnections, re-connections, double connections, this kind of stuff...
// TODO errors

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
#include "db_connection.h"

using namespace sql;
using namespace std;
using namespace db;

int main()
{
	try
	{
		//sql::Driver* driver;
		//sql::Connection* connection;
		// Get driver
		//driver = get_driver_instance();
		//connection = driver->connect("tcp://151.97.9.112:3308", "f4k", "f4kpwd");
		//connection->setAutoCommit(false);
		//connection->setSchema("f4k_taiwan");
		//DBConnection conn("tcp://151.97.9.112:3308", "f4k", "f4kpwd", "f4k_taiwan");7
		Log::debug(0) << "before constr calling" << endl;
		DBConnection conn("db_config.xml");
		conn.query("SELECT COUNT(*) AS c FROM processed_videos");
		conn.fetchRow();
		cout << "Count: " << conn.getField<int>("c") << endl;
		//cout << "connected." << endl;
	}
	catch(MyException& e)
	{
		cout << "error: " << e.what() << endl;
	}
	catch(SQLException& e)
	{
		cout << "error: " << e.what() << endl;
	}
}
