#ifndef _DEVICEMANAGER__H_
#define _DEVICEMANAGER__H_

#include <thread>
#include <mutex>
#include <unordered_map>

#include <plotting/RealTimePlot.h>
#include <serial/SerialDevice.h>

namespace RTPlot
{
	class DeviceComponents
	{
	public:
		std::thread   thread;
		std::mutex    mutex;
		RealTimePlot* plotter;
		SerialDevice* serialDevice;
		uint8_t id = 0;
		bool exitFlag = false;

		DeviceComponents(const char* port) : plotter(new RealTimePlot), serialDevice(new SerialDevice(port))
		{
			thread = std::thread(serialReadingFunc);
		}

		~DeviceComponents(void)
		{
			exitFlag = true;
			thread.join();
			delete plotter;
			delete serialDevice;
		}

		void serialReadingFunc(void)
		{
			while (!exitFlag)
			{
				mutex.lock();
				if (serialDevice->recieve())
				{
					*(plotter->getDataPtr()) = serialDevice->getMessage();
				}
				mutex.unlock();
			}
		}
	};

	class DeviceManager
	{
		std::vector<DeviceComponents*> components;

	public:
		~DeviceManager(void) { for (auto i : components) delete i; }

		// Actions
		void AddDevice(const char* port) { components.push_back(new DeviceComponents(port)); components[components.size() - 1]->id = components.size() - 1; }
		void RemoveDevice(uint8_t i)     { components.erase(components.begin() + i); }

		void plotDevice(uint8_t id)      { components[id]->plotter->plot(); }
		void plotAll(void)               { for (uint8_t i = 0; i < components.size(); i++) plotDevice(i); }
	};
}

#endif