#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port)
{
	components.push_back(new DeviceComponents(port));
	components[components.size() - 1]->id = components.size() - 1;
	components[components.size() - 1]->plotter->SetID(components[components.size() - 1]->id);
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete components[i];
	components.erase(components.begin() + i);
}

void RTPlot::DeviceManager::PlotDevice(uint8_t id, RTPlot::Graphics* graphicsPtr)
{
	if (components[id]->plotter->GetPlotExitFlag())
		components[id]->plotter->Plot(components[id]->serialDevice->GetPort()->GetName(), components[id]->GetPlotFlagPtr(), graphicsPtr);
}

void RTPlot::DeviceManager::PlotDevices(RTPlot::Graphics* graphicsPtr)
{
	for (uint8_t i = 0; i < components.size(); i++)
		PlotDevice(i, graphicsPtr);
}
