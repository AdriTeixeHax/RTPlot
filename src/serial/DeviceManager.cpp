#include <serial/DeviceManager.h>

void RTPlot::DeviceManager::AddDevice(const char* port, Graphics* graphicsPtr)
{
	// Error checking.
	if (!graphicsPtr) std::cerr << "[DeviceManager]: Graphics pointer is not valid." << std::endl;
	
	// Create the new component and asign it a serial port and ID.
	components.push_back(new DeviceComponent(port, graphicsPtr));
	uint8_t newID = components.size() - 1;
	components[newID]->SetID(newID);

	// Assign the plotters' IDs.
	DeviceComponent* newComponent = components[components.size() - 1];
	for (uint8_t i = 0; i < newComponent->GetPlottersSize(); i++)
		newComponent->SetPlotterID(i, newID * 10 + i); // Plotter ID 11 = Plotter 1, ID 1
}

void RTPlot::DeviceManager::RemoveDevice(uint8_t i)
{
	delete components[i];
	components.erase(components.begin() + i);
}

void RTPlot::DeviceManager::PlotDevice(uint8_t id, uint8_t plotterID)
{
	static const std::string prefix = "\\\\.\\"; // Part of the string to strip
	
	if (floor(plotterID / 10) != id)
	{
		std::cerr << "[DeviceManager]: Invalid plotter ID specified: floor(plotterID / 10) != id" << std::endl;
		return;
	}
	
	if (!components[id]->GetPlotExitFlag(id)) return;

	std::string name = components[id]->GetPortName();
	size_t pos; // Iterator
	while ((pos = name.find(prefix)) != std::string::npos)
		name.erase(pos, prefix.length());

	char addedString[6];
	snprintf(addedString, sizeof(addedString), " - %d", plotterID);
	name.append(addedString);

	components[id]->Plot(plotterID - id * 10, name); // Undoes the plotterID calculation
}

void RTPlot::DeviceManager::PlotAllDevices(void)
{
	for (uint8_t i = 0; i < components.size(); i++)
	{
		for (uint8_t j = 0; j < components.at(i)->GetPlottersSize(); j++)
			PlotDevice(i, 10 * i + j);
	}
}
