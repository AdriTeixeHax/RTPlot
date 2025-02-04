#include <serial/DeviceComponent.h>
#include <RTPlotVars.h>

RTPlot::DeviceComponent::DeviceComponent(const char* port, Graphics* graphicsPtr) :
	plotter(graphicsPtr),
	serialDevice(new SerialDevice(port))
{
    this->graphicsPtr = graphicsPtr;  
	thread = std::thread(&DeviceComponent::SerialFunc, this);
}

RTPlot::DeviceComponent::~DeviceComponent(void)
{
    exitThreadFlag = true;
    thread.join();
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

void RTPlot::DeviceComponent::SerialFunc(void)
{
    while (!exitThreadFlag)
    {
        serialDevice->Recieve();
        plotter.SetDataToPlot(serialDevice->GetReading());

        if (sendCommand)
        {
            serialDevice->Send(command);
            sendCommand = false;
        }
    }
}