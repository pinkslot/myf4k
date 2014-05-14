// Machine information

#ifndef UTILS_MACHINE_UTILS_H
#define UTILS_MACHINE_UTILS_H

#include <sigar.h>
#include <string>

/** Provide information on running machine.
 */
class MachineUtils
{
	private:

	// Sigar objects
	static sigar_t* sigar;

	static sigar_t* getSigar();

	public:

	/** Default constructor.
	 */
	MachineUtils();

	/** Destructor.
	 */
	virtual ~MachineUtils();

	/** Get CPU model.
	 */
	static std::string getCPUModel();		// CPU model

	/** Get CPU speed in MHz.
	 */
	static int getCPUSpeed();			// CPU speed (frequency in MHz)

	/** Get number of CPU cores.
	 */
	static int getCores();				// Number of cores

	/** Get CPU cache size in KB.
	 */
	static int getCacheSize();			// Cache size (in kB)

	/** Get RAM size in MB.
	 */
	static int getRAM();				// RAM size (in MB)

	/** Get RAM frequency in MHz.
	 */
	static int getRamFrequency();			// RAM frequency (in MHz)

	/** Get amount of RAM currently used by the process in bytes.
	 */
	static long getRAMUsage();			// RAM currently used by the process (in bytes)

};

#endif
