// Algorithm which returns its information string fields (used to convert from db::Algorithm to alg::Algorithm)

#ifndef ALG_CUSTOM_ALGORITHM_H
#define ALG_CUSTOM_ALGORITHM_H

#include "parameters.h"
#include <vector>

namespace alg
{

	class CustomAlgorithm : public Algorithm
	{
	
		private:

		// User-provided information
		string _name;
		string _type;
		string _description;
		string _version;
		int _executionTime;
		string _descriptionExecutionTime;
		int _ram;
		string _input;
		string _output;
		string _settings;

		public:

		// Set algorithm data
		inline void setName(string n) { _name = n; }
		inline void setType(string t) { _type = t; }
		inline void setDescription(string d) { _description = d; }
		inline void setVersion(string v) { _version = v; }
		inline void setExecutionTime(int e) { _executionTime = e; }
		inline void setDescriptionExecutionTime(string d) { _descriptionExecutionTime = d; }
		inline void setRam(int r) { _ram = r; }
		inline void setInput(string i) { _input = i; }
		inline void setOutput(string o) { _output = o; }
		
		// Information on the algorithm - read from private variables
		inline string name() const { return _name; }						// e.g. gmm, apmm
		inline string type() const { return _type; }						// e.g. detection, tracking, recognition
		inline string description() const { return _description; }				// e.g. algorithm description
		inline string version() const { return _version; }					// e.g. 1.2
		inline int executionTime() const { return _executionTime; }				// time (in milliseconds) to process an input unit (e.g. frame for detection algorithms, object for tracking algorithms)
		inline string descriptionExecutionTime() const { return _descriptionExecutionTime; }	// description of how the execution time is computer
		inline int ram() const { return _ram; }							// average amount of RAM used by the algorithm in MB
		inline string input() const { return _input; }						// e.g. Mat, vector<CvBlob>
		inline string output() const { return _output; }					// e.g. vector<TrackedObject>

		// Manage settings
		inline string settings() const { return _settings; }
		inline void setSettings(string s) { _settings = s; }
		
	};

}

#endif
