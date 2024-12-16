#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port, Graphics* graphicsPtr)
{
	// Error checking.
	if (!graphicsPtr) std::cerr << "[DeviceManager]: Graphics pointer is not valid." << std::endl;
	
	// Create the new component and asign it a serial port and ID.
	components.push_back(new DeviceComponent(port, graphicsPtr));
	uint8_t newID = components.size() - 1;
	components[newID]->SetID(newID);
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete components[i];
	components.erase(components.begin() + i);
}

void RTPlot::DeviceManager::PlotDevice(uint8_t id)
{
	static const std::string prefix = "\\\\.\\"; // Part of the string to strip
	
	if (!components[id]->GetPlotExitFlag(id)) return;

	std::string name = components[id]->GetPortName();
	size_t pos; // Iterator
	while ((pos = name.find(prefix)) != std::string::npos)
		name.erase(pos, prefix.length());

	components[id]->Plot(name);
}

void RTPlot::DeviceManager::PlotAllDevices(void)
{
	for (uint8_t i = 0; i < components.size(); i++)
	{
		PlotDevice(i);
	}
}
