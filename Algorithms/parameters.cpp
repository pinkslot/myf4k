// Class for algorithm parameters

#include "parameters.h"
#include <string_utils.h>
#include <ticpp.h>

using namespace ticpp;

namespace alg
{

	// Get parameter list
	vector<string> Parameters::list() const
	{
		// Initialize list
		vector<string> param_list;
		// Go through all fields
		for(map<string,string>::const_iterator it = data.begin(); it != data.end(); it++)
		{
			// Add field to list
			param_list.push_back(it->first);
		}
		// Return list
		return param_list;
	}
		
	// Copy parameters
	void Parameters::read(const Parameters& parameters)
	{
		// Get list
		vector<string> params = parameters.list();
		// Add each parameter
		for(vector<string>::iterator it = params.begin(); it != params.end(); it++)
		{
			set<string>(*it, parameters.get<string>(*it));
		}
	}

	// Read from XML
	void Parameters::readFromXML(string path)
	{
		// Read XML
		try
		{
			Document doc(path);
			doc.LoadFile();
			// Read all parameters
			Node* video = doc.FirstChild("parameters");
			Iterator<Element> parameter("parameter");
			for(parameter = parameter.begin(video); parameter != parameter.end(); parameter++)
			{
				// Read parameter
				string parameter_name = parameter->GetAttribute<string>("name"); 
				string parameter_value = parameter->GetText();
				// Save it
				set<string>(parameter_name, parameter_value);
			}
		}
		catch(ticpp::Exception& ex)
		{
			stringstream ss;
			ss << "XML error: " << ex.what() << endl;
			throw MyException(ss.str());
		}
	}
		
	// Read from string (k1=v1,k2=v2,...)
	void Parameters::readFromString(string str)
	{
		// Check valid string
		// The last two are needed if someone needs to pass an empty string from a bash script
		if(str.size() == 0 || str == "\"\"" || str == "''")
		{
			return;
		}
		// Split parameters
		vector<string> kv_pairs = splitString(str, ',');
		// Go through all pairs
		for(vector<string>::iterator it = kv_pairs.begin(); it != kv_pairs.end(); it ++)
		{
			// Split key and value
			vector<string> kv_pair = splitString(*it, '=');
			// Save parameter
			set<string>(kv_pair[0], kv_pair[1]);
		}
	}

}
