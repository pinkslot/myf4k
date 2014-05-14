// FIXME manage memory allocated by convertToAlgInstance

#include "algorithm.h"
#include <Algorithms/custom_algorithm.h>

namespace db
{

	// Setup entity fields
	void Algorithm::setupFields()
	{
		// Set table
		setTable("algorithm");
		// Add fields
		addField("id");
		addField("name");
		addField("type");
		addField("description");
		addField("version");
		addField("execution_time");
		addField("description_execution_time");
		addField("ram");
		addField("language");
		addField("input");
		addField("output");
		// Set key
		addKey("id");
		// Set lookup fields
		addLookupField("name");
		addLookupField("type");
		addLookupField("version");
	}
		
	// Fill fields with actual algorithm
	void Algorithm::fillFields(const alg::Algorithm& algorithm)
	{
		setField<string>("name", algorithm.name());
		setField<string>("type", algorithm.type());
		setField<string>("description", algorithm.description());
		setField<string>("version", algorithm.version());
		setField<int>("execution_time", algorithm.executionTime());
		setField<string>("description_execution_time", algorithm.descriptionExecutionTime());
		setField<int>("ram", algorithm.ram());
		setField<string>("language", algorithm.language());
		setField<string>("input", algorithm.input());
		setField<string>("output", algorithm.output());
	}
		
	// Convert to alg::Algorithm
	alg::Algorithm* Algorithm::convertToAlgInstance()
	{
		// Create new custom algorithm
		alg::CustomAlgorithm *algorithm = new alg::CustomAlgorithm();
		// Fill data
		algorithm->setName(getField<string>("name"));
		algorithm->setType(getField<string>("type"));
		algorithm->setDescription(getField<string>("description"));
		algorithm->setVersion(getField<string>("version"));
		algorithm->setExecutionTime(getField<int>("execution_time"));
		algorithm->setDescriptionExecutionTime(getField<string>("description_execution_time"));
		algorithm->setRam(getField<int>("ram"));
		algorithm->setInput(getField<string>("input"));
		algorithm->setOutput(getField<string>("output"));
		// Return algorithm
		return algorithm;
	}

}
