#include "xmlconfig.h"
#include <ticpp.h>
#include <log.h>
#include <sstream>
#include <my_exception.h>
#include <preprocessing_chooser.h>
#include <detection_chooser.h>
#include <blob_processing_chooser.h>
#include <tracker_chooser.h>
#include <detection_evaluation_chooser.h>
#include <tracking_evaluation_chooser.h>

using namespace ticpp;
using namespace std;
	
// Load from XML file
void XMLConfig::readFile(string file_path)
{
	// Clear data
	preprocessing.clear();
	detection.clear();
	blob_processing.clear();
	tracking.clear();
	detection_evaluation.clear();
	tracking_evaluation.clear();
	preprocessing_settings.clear();
	detection_settings.clear();
	blob_processing_settings.clear();
	tracking_settings.clear();
	detection_evaluation_settings.clear();
	tracking_evaluation_settings.clear();
	// Read XML
	try
	{
		Document doc(file_path);
		doc.LoadFile();
		// Get first node
		Node* configuration = doc.FirstChild("configuration");
		// Read all nodes
		Iterator<Element> algorithm;
		for(algorithm = algorithm.begin(configuration); algorithm != algorithm.end(); ++algorithm)
		{
			// Get type and name
			string type = ((Node*)(&*algorithm))->Value();
			string name = algorithm->GetAttribute<string>("name");
			// Check type
			if(type != "frame_preproc" && type != "detection" && type != "post_processing" && type != "blob_filtering" && type != "tracking" && type != "result_processing" && type != "detection_evaluation" && type != "tracking_evaluation")
			{
				throw MyException("XMLConfig: undefined algorithm type '" + type + "'");
			}
			// Read parameters and build parameter string
			stringstream params_ss;
			Iterator<Element> param("parameter");
			for(param = param.begin(&*algorithm); param != param.end(); param++)
			{
				// Add to string
				if(params_ss.str().size() == 0)
				{
					params_ss << param->GetAttribute<string>("name") << "=" << param->GetText();
				}
				else
				{
					params_ss << "," << param->GetAttribute<string>("name") << "=" << param->GetText();
				}
			}
			string params = params_ss.str();
			// Set data
			if(type == "frame_preproc")
			{
				preprocessing.push_back(name);
				preprocessing_settings.push_back(params);
			}
			else if(type == "detection")
			{
				detection = name;
				detection_settings = params;
			}
			else if(type == "post_processing")
			{
				post_processing.push_back(name);
				post_processing_settings.push_back(params);
			}
			else if(type == "blob_filtering")
			{
				blob_processing.push_back(name);
				blob_processing_settings.push_back(params);
			}
			else if(type == "tracking")
			{
				tracking = name;
				tracking_settings = params;
			}
			else if(type == "result_processing")
			{
				result_processing.push_back(name);
				result_processing_settings.push_back(params);
			}
			else if(type == "detection_evaluation")
			{
				detection_evaluation = name;
				detection_evaluation_settings = params;
			}
			else if(type == "tracking_evaluation")
			{
				tracking_evaluation = name;
				tracking_evaluation_settings = params;
			}
		}
	}
	catch(ticpp::Exception& ex)
	{
		stringstream ss;
		ss << "XML error: " << ex.what() << endl;
		throw MyException(ss.str());
	}
}
