#include <serial/DeviceComponent.h>
#include <RTPlotVars.h>

RTPlot::DeviceComponent::DeviceComponent(const char* port, Graphics* graphicsPtr) :
	plotters(),
	serialDevice(new SerialDevice(port))
{
    for (uint8_t i = 0; i < RTPLOT_DATA_NUM; i++)
        plotters.push_back(new RealTimePlot(graphicsPtr));

	thread = std::thread(&DeviceComponent::SerialReadingFunc, this);
}

RTPlot::DeviceComponent::~DeviceComponent(void)
{
    exitThreadFlag = true;
    thread.join();

    mutex.lock();
        for (auto i : plotters)
            delete i;
        delete serialDevice;
    mutex.unlock();
}

void RTPlot::DeviceComponent::SerialReadingFunc(void)
{
    while (!exitThreadFlag)
    {
        mutex.lock();
        serialDevice->Recieve();
        mutex.unlock();

        for (uint8_t i = 0; i < plotters.size(); i++)
            plotters.at(i)->SetDataToPlot(serialDevice->GetValue(i));
    }
}