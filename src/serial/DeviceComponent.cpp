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
	mutex.lock();
	delete serialDevice;
}

void RTPlot::DeviceComponent::SerialReadingFunc(void)
{
	while (!exitFlag)
	{
		mutex.lock();
		if (!serialDevice) { std::cerr << "[DeviceComponent]: Serial Device pointer does not exist." << std::endl; break; }
		serialDevice->Recieve();
		mutex.unlock();
		//for (uint8_t i = 0; i < RTPLOT_DATA_NUM; i++)
		//{
		//	plotter->GetDataPtr()[i] = serialDevice->GetMsgArray()->at(i);
		//}
		//if (sendMsgFlag)
		//{
		//	serialDevice->Send(writingMsg);
		//	sendMsgFlag = false;
		//}
	}
}