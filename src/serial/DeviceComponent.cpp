#include <serial/DeviceComponent.h>
#include <RTPlotVars.h>

RTPlot::DeviceComponent::DeviceComponent(const char* port, Graphics* graphicsPtr) :
	plotters(),
	serialDevice(new SerialDevice(port))
{
    //for (uint8_t i = 0; i < RTPLOT_DATA_NUM - 1; i++) // - 1 because the first value is the time
    //    plotters.push_back(new RealTimePlot(graphicsPtr));
    this->graphicsPtr = graphicsPtr;  
    AddPlotter(); // Add a first plotter
	thread = std::thread(&DeviceComponent::SerialReadingFunc, this);
}

RTPlot::DeviceComponent::~DeviceComponent(void)
{
    exitThreadFlag = true;
    thread.join();

    for (auto i : plotters)
        delete i;
    delete serialDevice;
}

std::string RTPlot::DeviceComponent::GetPortNameGUI(void) const 
{
    std::string name = this->GetPortName();
    static const std::string prefix = "\\\\.\\"; // Part of the string to strip
    size_t pos; // Iterator
    while ((pos = name.find(prefix)) != std::string::npos)
        name.erase(pos, prefix.length());
    return name;
}

void RTPlot::DeviceComponent::SerialReadingFunc(void)
{
    while (!exitThreadFlag)
    {
        serialDevice->Recieve();

        for (uint8_t i = 0; i < plotters.size(); i++) // plotters.size() = RTPLOT_DATA_NUM - 1
            plotters.at(i)->SetDataToPlot(serialDevice->GetReading());
    }
}