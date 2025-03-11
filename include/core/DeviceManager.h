/// DeviceManager.h - Header file to manage a list of devices. This is the object that lives inside the main application and manages all the devices connected to the computer.

#ifndef _DEVICEMANAGER__H_
#define _DEVICEMANAGER__H_

#include <vector>

#include <serial/SerialPlotter.h>

namespace RTPlot
{
	class DeviceManager
	{
		std::vector<SerialPlotter*> devices;

	public:
		~DeviceManager(void) { for (auto i : devices) delete i; }	

		// Getters
		size_t Size(void) const { return devices.size(); }
		const std::vector<SerialPlotter*>& GetComponents(void) { return devices; }
		SerialPlotter* operator[](size_t i) { return devices.at(i); }

		// Actions
		void AddDevice(const char* port, std::string& logMsg);
		void RemoveDevice(size_t i);

		void PlotAllDevices(void);
	};
}

#endif