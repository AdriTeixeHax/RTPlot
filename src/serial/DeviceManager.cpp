#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port, Graphics* graphicsPtr)
{
	// Error checking.
	if (!graphicsPtr) std::cerr << "[DeviceManager]: Graphics pointer is not valid." << std::endl;
	
	// Create the new component and asign it the serial port.
	components.push_back(new DeviceComponent(port, graphicsPtr));
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete components[i];
	components.erase(components.begin() + i);
}

void RTPlot::DeviceManager::PlotAllDevices(void)
{
	for (uint8_t i = 0; i < components.size(); i++)
	{
		if (!components[i]->GetPlotExitFlag(i)) return;

		static const std::string prefix = "\\\\.\\"; // Part of the string to strip
		std::string name = components[i]->GetPortName();
		size_t pos; // Iterator
		while ((pos = name.find(prefix)) != std::string::npos)
			name.erase(pos, prefix.length());

		components[i]->Plot(name);
	}
}
