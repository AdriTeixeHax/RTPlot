#include <DeviceManager.h>

#include <RTPlotFunctions.h>

void RTPlot::DeviceManager::AddDevice(const char* port, std::string& logMsg)
{
	// Create the new component and asign it the serial port.
	devices.push_back(new SerialPlotter(port, &logMsg));
}

void RTPlot::DeviceManager::RemoveDevice(size_t i)
{
	delete devices.at(i);
	devices.erase(devices.begin() + i);
}

void RTPlot::DeviceManager::PlotAllDevices(void)
{
	for (uint8_t i = 0; i < devices.size(); i++)
	{
		if (!devices.at(i)->GetPlotExitFlag(i)) return;
		std::string name = GUIPortNameCalc(devices.at(i)->GetPortName());
		devices.at(i)->Plot(name);
	}
}
