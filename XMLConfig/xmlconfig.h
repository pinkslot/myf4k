// Read software configuration from XML
#ifndef XML_CONFIG_H
#define XML_CONFIG_H

#include <vector>
#include <string>

class XMLConfig
{
	private:

	// Algorithm names
	std::vector<std::string> preprocessing;
	std::string detection;
	std::vector<std::string> post_processing;
	std::vector<std::string> blob_processing;
	std::string tracking;
	std::vector<std::string> result_processing;
	std::string detection_evaluation;
	std::string tracking_evaluation;

	// Algorithm parameters
	std::vector<std::string> preprocessing_settings;
	std::string detection_settings;
	std::vector<std::string> post_processing_settings;
	std::vector<std::string> blob_processing_settings;
	std::string tracking_settings;
	std::vector<std::string> result_processing_settings;
	std::string detection_evaluation_settings;
	std::string tracking_evaluation_settings;

	public:
	
	// Constructor
	XMLConfig() {}

	// Destructor
	virtual ~XMLConfig() {}

	// Load from XML file
	void readFile(std::string file_path);

	// Read enabled algorithms
	inline std::vector<std::string> getPreProcessing() { return preprocessing; }
	inline std::string getDetection() { return detection; }
	inline std::vector<std::string> getPostProcessing() { return post_processing; }
	inline std::vector<std::string> getBlobProcessing() { return blob_processing; }
	inline std::string getTracking() { return tracking; }
	inline std::vector<std::string> getResultProcessing() { return result_processing; }
	inline std::string getDetectionEvaluation() { return detection_evaluation; }
	inline std::string getTrackingEvaluation() { return tracking_evaluation; }
	
	// Read algorithm parameters
	inline std::vector<std::string> getPreProcessingSettings() { return preprocessing_settings; }
	inline std::string getDetectionSettings() { return detection_settings; }
	inline std::vector<std::string> getPostProcessingSettings() { return post_processing_settings; }
	inline std::vector<std::string> getBlobProcessingSettings() { return blob_processing_settings; }
	inline std::string getTrackingSettings() { return tracking_settings; }
	inline std::vector<std::string> getResultProcessingSettings() { return result_processing_settings; }
	inline std::string getDetectionEvaluationSettings() { return detection_evaluation_settings; }
	inline std::string getTrackingEvaluationSettings() { return tracking_evaluation_settings; }

};

#endif
