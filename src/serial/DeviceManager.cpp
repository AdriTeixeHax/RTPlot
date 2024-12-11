#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port, Graphics* graphicsPtr)
{
	if (!graphicsPtr) std::cerr << "[DeviceManager]: Graphics pointer is not valid." << std::endl;
	
	components.push_back(new DeviceComponent(port, graphicsPtr));
	uint8_t newID = components.size() - 1;
	components[components.size() - 1]->SetID(newID);
	components[components.size() - 1]->SetPlotterID(newID);
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete components[i];
	components.erase(components.begin() + i);
}

void RTPlot::DeviceManager::PlotDevice(uint8_t id)
{
	if (components[id]->GetPlotExitFlag())
		components[id]->Plot(components[id]->GetPortName());
}

void RTPlot::DeviceManager::PlotAllDevices(void)
{
	for (uint8_t i = 0; i < components.size(); i++)
		PlotDevice(i);
}
