#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port, Graphics* graphicsPtr)
{
	// Error checking.
	if (!graphicsPtr) std::cerr << "[DeviceManager]: Graphics pointer is not valid." << std::endl;
	
	// Create the new component and asign it the serial port.
	devices.push_back(new SerialPlotter(port, graphicsPtr));
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete devices[i];
	devices.erase(devices.begin() + i);
}

void RTPlot::DeviceManager::PlotAllDevices(void)
{
	for (uint8_t i = 0; i < devices.size(); i++)
	{
		if (!devices[i]->GetPlotExitFlag(i)) return;

		std::string name = devices[i]->GetPortName();
		size_t pos; // Iterator
		while ((pos = name.find(LONG_COM_PORT_PREFIX)) != std::string::npos)
			name.erase(pos, strlen(LONG_COM_PORT_PREFIX));

		devices[i]->Plot(name);
	}
}
