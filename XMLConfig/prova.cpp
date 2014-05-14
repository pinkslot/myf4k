#include "xmlconfig.h"
#include <my_exception.h>
#include <iostream>

using namespace std;

int main()
{
	try
	{
		XMLConfig config;
		config.readFile("sw_config.xml");
		vector<string> preprocessing = config.getPreProcessing();
		vector<string> preprocessing_settings = config.getPreProcessingSettings();
		for(unsigned int i=0; i<preprocessing.size(); ++i)
		{
			cout << "Preprocessing: " << preprocessing[i];
			if(i < preprocessing_settings.size())
			{
				cout << " (" << preprocessing_settings[i] << ")" << endl;
			}
		}
		cout << "Detection: " << config.getDetection() << " (" << config.getDetectionSettings() << ")" << endl;
		vector<string> blob_processing = config.getBlobProcessing();
		vector<string> blob_processing_settings = config.getBlobProcessingSettings();
		for(unsigned int i=0; i<blob_processing.size(); ++i)
		{
			cout << "Blob processing: " << blob_processing[i];
			if(i < blob_processing_settings.size())
			{
				cout << " (" << blob_processing_settings[i] << ")" << endl;
			}
		}
		cout << "Tracking: " << config.getTracking() << " (" << config.getTrackingSettings() << ")" << endl;
		cout << "Detection evaluation: " << config.getDetectionEvaluation() << " (" << config.getDetectionEvaluationSettings() << ")" << endl;
		cout << "Tracking evaluation: " << config.getTrackingEvaluation() << " (" << config.getTrackingEvaluationSettings() << ")" << endl;
	}
	catch(MyException& e)
	{
		cout << e.what() << endl;
	}
}
