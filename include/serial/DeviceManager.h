#ifndef _DEVICEMANAGER__H_
#define _DEVICEMANAGER__H_

#include <vector>

#include "DeviceComponents.h"

namespace RTPlot
{
	class DeviceManager
	{
		std::vector<DeviceComponents*> components;

	public:
		~DeviceManager(void) { for (auto i : components) delete i; }

		// Getters
		uint8_t Size(void) { return components.size(); }
		DeviceComponents* operator[](uint8_t i) { return components[i]; }

		// Actions
		void AddDevice(const char* port);
		void RemoveDevice(uint8_t i);

		void PlotDevice(uint8_t id, RTPlot::Graphics* graphicsPtr);
		void PlotDevices(RTPlot::Graphics* graphicsPtr);
	};
}

#endif