#include <serial/DeviceComponents.h>

RTPlot::DeviceComponents::DeviceComponents(const char* port) :
	plotter(new RealTimePlot(&reading, &writingMsg)),
	serialDevice(new SerialDevice(port))
{
	thread = std::thread(&DeviceComponents::SerialReadingFunc, this);
}

RTPlot::DeviceComponents::~DeviceComponents(void)
{
	exitFlag = true;
	thread.join();
	delete plotter;
	delete serialDevice;
}

void RTPlot::DeviceComponents::SerialReadingFunc(void)
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