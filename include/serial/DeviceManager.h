#ifndef _DEVICEMANAGER__H_
#define _DEVICEMANAGER__H_

#include <vector>

#include <serial/DeviceComponent.h>

namespace RTPlot
{
	class DeviceManager
	{
		std::vector<DeviceComponent*> components;

	public:
		~DeviceManager(void) { for (auto i : components) delete i; }	

		// Getters
		uint8_t Size(void) { return components.size(); }
		DeviceComponent* operator[](uint8_t i) { return components.at(i); }

		// Actions
		void AddDevice(const char* port, Graphics* graphicsPtr);
		void RemoveDevice(uint8_t i);

		void PlotDevice(uint8_t id, uint8_t plotterID);
		void PlotAllDevices(void);
	};
}

#endif