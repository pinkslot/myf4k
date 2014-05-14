// FIXME currently, we assume we only have one CPU with multiple cores

#include "machine_utils.h"
#include <my_exception.h>
// Debug includes
#include <iostream>

using namespace std;

sigar_t* MachineUtils::sigar = NULL;

// Constructor
MachineUtils::MachineUtils()
{
}

// Destructor; frees sigar if necessary
MachineUtils::~MachineUtils()
{
	if(sigar != NULL)
	{
		sigar_close(sigar);
	}
	sigar = NULL;
}

sigar_t* MachineUtils::getSigar()
{
	if(sigar == NULL)
	{
		if(sigar_open(&sigar) != SIGAR_OK)
		{
			throw MyException("Couldn't initialize Sigar for machine information retrieval.");
		}
	}
	return sigar;
}

string MachineUtils::getCPUModel()
{
	// Initialize Sigar
	getSigar();
	// Get CPU list
	sigar_cpu_info_list_t cpu_infos;
	if(sigar_cpu_info_list_get(sigar, &cpu_infos) != SIGAR_OK || cpu_infos.number == 0)	throw MyException("Couldn't get CPU list.");
	// Store result
	string result = cpu_infos.data[0].model;
	// Free memory
	sigar_cpu_info_list_destroy(sigar, &cpu_infos);
	// Return result
	return result;
}

int MachineUtils::getCPUSpeed()
{
	// Initialize Sigar
	getSigar();
	// Get CPU list
	sigar_cpu_info_list_t cpu_infos;
	if(sigar_cpu_info_list_get(sigar, &cpu_infos) != SIGAR_OK || cpu_infos.number == 0)	throw MyException("Couldn't get CPU list.");
	// Store result
	float result = cpu_infos.data[0].mhz;
	// Free memory
	sigar_cpu_info_list_destroy(sigar, &cpu_infos);
	// Return result
	return result;
}

int MachineUtils::getCores()
{
	// Initialize Sigar
	getSigar();
	// Get CPU list
	sigar_cpu_info_list_t cpu_infos;
	if(sigar_cpu_info_list_get(sigar, &cpu_infos) != SIGAR_OK || cpu_infos.number == 0)	throw MyException("Couldn't get CPU list.");
	// Store result
	int result = cpu_infos.data[0].total_cores;
	// Free memory
	sigar_cpu_info_list_destroy(sigar, &cpu_infos);
	// Return result
	return result;
}

int MachineUtils::getCacheSize()
{
	// Initialize Sigar
	getSigar();
	// Get CPU list
	sigar_cpu_info_list_t cpu_infos;
	if(sigar_cpu_info_list_get(sigar, &cpu_infos) != SIGAR_OK || cpu_infos.number == 0)	throw MyException("Couldn't get CPU list.");
	// Store result
	int result = cpu_infos.data[0].cache_size;
	// Free memory
	sigar_cpu_info_list_destroy(sigar, &cpu_infos);
	// Return result
	return result;
}

int MachineUtils::getRAM()
{
	// Initialize Sigar
	getSigar();
	// Get memory info
	sigar_mem_t mem_info;
	if(sigar_mem_get(sigar, &mem_info) != SIGAR_OK)	throw MyException("Couldn't get CPU list.");
	// Return result
	return mem_info.ram;
}

int MachineUtils::getRamFrequency()
{
	return -1;
}

long MachineUtils::getRAMUsage()
{
	// Initialize Sigar
	getSigar();
	// Get process memory info
	sigar_proc_mem_t proc_mem_info;
	sigar_pid_t pid = sigar_pid_get(sigar);
	if(sigar_proc_mem_get(sigar, pid, &proc_mem_info) != SIGAR_OK)	throw MyException("Couldn't get process memory information.");
	// Return result
	return proc_mem_info.resident;
}
