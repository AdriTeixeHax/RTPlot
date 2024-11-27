#include <serial/DeviceComponent.h>

RTPlot::DeviceComponent::DeviceComponent(const char* port, Graphics* graphicsPtr) :
	plotter(new RealTimePlot(&reading, &writingMsg, graphicsPtr)),
	serialDevice(new SerialDevice(port))
{
	thread = std::thread(&DeviceComponent::SerialReadingFunc, this);
}

RTPlot::DeviceComponent::~DeviceComponent(void)
{
	exitFlag = true;
	thread.join();
	delete plotter;
	delete serialDevice;
}

void RTPlot::DeviceComponent::SerialReadingFunc(void)
{
	while (!exitFlag)
	{
		mutex.lock();
		if (serialDevice->Recieve())
		{
			*(plotter->GetDataPtr()) = serialDevice->GetMsg();
		}
		if (sendMsgFlag)
		{
			serialDevice->Send(writingMsg);
			sendMsgFlag = false;
		}
		mutex.unlock();
	}
}