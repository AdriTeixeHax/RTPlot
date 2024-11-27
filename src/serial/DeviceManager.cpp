#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port, Graphics* graphicsPtr)
{
	components.push_back(new DeviceComponent(port, graphicsPtr));
	components[components.size() - 1]->id = components.size() - 1;
	components[components.size() - 1]->plotter->SetID(components[components.size() - 1]->id);
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete components[i];
	components.erase(components.begin() + i);
}

void RTPlot::DeviceManager::PlotDevice(uint8_t id)
{
	if (components[id]->plotter->GetPlotExitFlag())
		components[id]->plotter->Plot(components[id]->serialDevice->GetPort()->GetName(), components[id]->GetPlotFlagPtr());
}

void RTPlot::DeviceManager::PlotDevices(void)
{
	for (uint8_t i = 0; i < components.size(); i++)
		PlotDevice(i);
}
