// Class for algorithm parameters
#ifndef ALG_PARAMETERS_H
#define ALG_PARAMETERS_H

#include <map>
#include <string>
#include <string_utils.h>
#include <vector>
#include <my_exception.h>

using namespace std;

namespace alg
{

	class Parameters
	{
		private:

		// Internal parameter map
		map<string, string> data;

		// Dynamic parameter adding flag
		bool flexible;

		public:

		// Constructor
		Parameters() :
			flexible(false)
		{
		}

		// Allow dynamic parameter adding
		inline void setFlexible(bool f) { flexible = f; }

		// Add parameter
		template<typename T>
		void add(string key, const T& default_value);

		// Set parameter
		template<typename T>
		void set(string key, const T& value);

		// Get parameter
		template<typename T>
		T get(string key) const;

		// Get parameter list
		vector<string> list() const;

		// Copy parameters - flexible might be required
		void read(const Parameters& parameters);

		// Read from XML
		void readFromXML(string path);
		
		// Read from string (k1=v1;k2=v2;...)
		void readFromString(string str);

	};

	// Add parameter
	template<typename T>
	void Parameters::add(string key, const T& default_value)
	{
		data[key] = convertToString<T>(default_value);
	}

	// Get parameter
	template<typename T>
	T Parameters::get(string key) const
	{
		// Check field exists
		map<string, string>::const_iterator it = data.find(key);
		if(it == data.end())
		{
			// Throw exception
			stringstream error;
			error << "Undefined parameter key '" << key << "'";
			throw MyException(error.str());
		}
		return convertFromString<T>(it->second);
	}

	// Set parameter
	template<typename T>
	void Parameters::set(string key, const T& value)
	{
		// Check field exists
		map<string, string>::iterator it = data.find(key);
		if(it == data.end())
		{
			// Check if we can add it now
			if(flexible)
			{
				add<T>(key, value);
				return;
			}
			else
			{
				// Throw exception
				stringstream error;
				error << "Undefined parameter key '" << key << "'";
				throw MyException(error.str());
			}
		}
		// Set value
		it->second = convertToString<T>(value);
	}

}

#endif
