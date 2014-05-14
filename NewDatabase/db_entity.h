// Represents a generic database entity, with a uniform mechanism for selection, insertion, deletion, etc
// TODO write extension guidelines
// TODO would it make sense to support more than one auto-increment fields?
// TODO use fields_set also when building queries

#ifndef DB_ENTITY_H
#define DB_ENTITY_H

#include "db_connection.h"
#include "db_utils.h"
#include <map>
#include <vector>
#include <sstream>

namespace db
{

	class DBEntity
	{
		private:

		protected:

		// Reference to connection
		DBConnection* connection;

		// Table name
		std::string table;

		// Entity fields, as string
		std::map<std::string, std::string> fields;

		// Flags to check whether a field has been assigned a value
		std::map<std::string, bool> fields_set;

		// Blob fields
		std::vector<std::string> blob_fields;

		// Keys; must be set after fields have been filled
		std::vector<std::string> keys;

		// Auto-increment key
		std::string auto_increment_key;
		bool auto_increment_key_set;

		// Read modifiers; used when a function has to be applied to the field value when reading data from the database
		std::map<std::string, std::string> read_modifiers;
		
		// Write modifiers; used when a function has to be applied to the field value when writing data from the database
		// The string pair writes the modified value as: pair->first + value + pair->second
		std::map<std::string, std::pair<std::string,std::string> > write_modifiers;

		// Auto-update fields (i.e., excluded in INSERT and UPDATE queries)
		std::vector<std::string> auto_update_fields;

		// Fields used to look up the entity (if empty, all non-key fields are used)
		std::vector<std::string> lookup_fields;

		public:

		// Constructor
		inline DBEntity() : connection(NULL), table(""), auto_increment_key(""), auto_increment_key_set(false) {}

		// Constructor; sets table
		inline DBEntity(std::string tab) : connection(NULL), table(tab), auto_increment_key(""), auto_increment_key_set(false) {}
		
		// Constructor; sets connection
		inline DBEntity(DBConnection* conn) : connection(conn), table(""), auto_increment_key(""), auto_increment_key_set(false) {}

		// Constructor; sets connection and table
		inline DBEntity(DBConnection* conn, std::string tab) : connection(conn), table(tab), auto_increment_key(""), auto_increment_key_set(false) {}

		// Destructor
		inline virtual ~DBEntity() {}

		// Constructor; sets connection
		inline void setConnection(DBConnection* conn) { connection = conn; }

		// Set table
		inline void setTable(std::string tab) { table = tab; }
		
		// Get table
		inline std::string getTable() const { return table; }
		
		// Add field
		inline void addField(std::string field) { fields[field] = ""; }

		// Add key
		void addKey(std::string key, bool auto_increment = true);

		// Set blob field
		void setBlob(std::string field);

		// Check if field is a blob field
		inline bool isBlobField(std::string field) const { return find(blob_fields.begin(), blob_fields.end(), field) != blob_fields.end(); }

		// Set auto-increment key
		void setAutoIncrementKey(std::string key);
		
		// Add read modifier
		void addReadModifier(std::string field, std::string modifier);
		
		// Add write modifier
		void addWriteModifier(std::string field, std::string mod_start, std::string mod_end);
		
		// Add auto-update fields
		void addAutoUpdateField(std::string field);

		// Add lookup fields
		void addLookupField(std::string field);

		// Get field list
		std::vector<std::string> getFields() const;

		// Check if a field has been set
		bool fieldIsSet(std::string field) const;

		// Check if a field has a read modifier
		bool fieldHasReadModifier(std::string field) const;

		// Get field name, including read modifier if present
		std::string getReadModifiedField(std::string field) const;
		
		// Get field value, including write modifier if present
		// Also, adds quotes to value
		std::string getWriteModifiedField(std::string field) const;

		// Get cached field value
		template<typename T>
		T getField(std::string field_name) const;
		
		// Set cached field value
		template<typename T>
		void setField(std::string field_name, const T& value);

		// Get values from database
		virtual void readFromDB();

		// Lookup entity and set key(s) -- DOESN'T CALL readFromDB()
		virtual bool lookup();
		
		// Write values to database
		virtual void writeToDB();
		
		// Insert new record into database
		virtual void insertToDB();

		// Delete record from database
		virtual void deleteFromDB();
	
		// Insert a group of records in a single query
		static void groupInsert(const std::vector<DBEntity*>& objects, db::DBConnection* connection, int bundle_size = 50);
	};

	// Get cached field value
	template<typename T>
	T DBEntity::getField(std::string field_name) const
	{
		// Check field exists
		std::map<std::string, std::string>::const_iterator it = fields.find(field_name);
		if(it == fields.end())
		{
			// Throw exception
			std::stringstream error;
			error << "Undefined field '" << field_name << "'";
			throw MyException(error.str());
		}
		return convertFromString<T>(it->second);
	}
	
	// Set cached field value
	template<typename T>
	void DBEntity::setField(std::string field_name, const T& value)
	{
		// Check field exists
		std::map<std::string, std::string>::iterator it = fields.find(field_name);
		if(it == fields.end())
		{
			// Throw exception
			std::stringstream error;
			error << "Undefined field '" << field_name << "'";
			throw MyException(error.str());
		}
		// Set value
		it->second = convertToString<T>(value);
		// Mark as set
		fields_set[field_name] = true;
	}
	

}

#endif
