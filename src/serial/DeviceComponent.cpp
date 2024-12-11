#include <serial/DeviceComponent.h>

RTPlot::DeviceComponent::DeviceComponent(const char* port, Graphics* graphicsPtr) :
	plotter(new RealTimePlot(graphicsPtr)),
	serialDevice(new SerialDevice(port))
{
	thread = std::thread(&DeviceComponent::SerialReadingFunc, this);
}

RTPlot::DeviceComponent::~DeviceComponent(void)
{
    exitThreadFlag = true;
    thread.join();

    mutex.lock();
    delete plotter;
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

        plotter->SetDataToPlot(serialDevice->GetMsgArray());
    }
}