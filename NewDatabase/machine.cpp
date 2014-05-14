#include "machine.h"
#include <machine_utils.h>
#include <sstream>

using namespace std;

namespace db
{

	// Setup entity fields
	void Machine::setupFields()
	{
		// Set table
		setTable("machine");
		// Add fields
		addField("id");
		addField("cpu_model");
		addField("speed_cpu");
		addField("number_cores");
		addField("cache_size");
		addField("ram");
		// Set key
		addKey("id");
	}

	void Machine::readFromSystem()
	{
		try
		{
			setField<string>("cpu_model", MachineUtils::getCPUModel());
			setField<int>("speed_cpu", MachineUtils::getCPUSpeed());
			setField<int>("number_cores", MachineUtils::getCores());
			setField<int>("cache_size", MachineUtils::getCacheSize());
			setField<int>("ram", MachineUtils::getRAM());
		}
		catch(MyException& e)
		{
			std::stringstream error;
			error << "Cannot get system data: " << e.what();
			throw MyException(error.str());
		}
	}
}
