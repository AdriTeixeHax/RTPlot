#ifndef _DEVICEMANAGER__H_
#define _DEVICEMANAGER__H_

#include <thread>

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
		bool          exitFlag = false;
		bool		  sendMsgFlag = false;
		uint8_t       id = 0;
		double        reading = 0;
		std::string   writingMsg = "";
		bool	      plotFlag = false;

		DeviceComponents(const char* port) : plotter(new RealTimePlot(&reading, &writingMsg)), serialDevice(new SerialDevice(port))
		{
			thread = std::thread(&DeviceComponents::serialReadingFunc, this);
		}

		~DeviceComponents(void)
		{
			exitFlag = true;
			thread.join();
			delete plotter;
			delete serialDevice;
		}

		bool  getPlotFlag(void)       { return plotFlag; }
		bool* getPlotFlagPtr(void)    { if (&plotFlag) return &plotFlag; }
		void  setPlotFlag(bool state) { plotFlag = state; }

		void serialReadingFunc(void)
		{
			while (!exitFlag)
			{
				mutex.lock();
				if (serialDevice->recieve())
				{
					*(plotter->getDataPtr()) = serialDevice->getMessage();
				}
				if (sendMsgFlag)
				{
					serialDevice->send(writingMsg);
					sendMsgFlag = false;
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

		// Getters
		uint8_t size(void) { return components.size(); }
		DeviceComponents* operator[](uint8_t i) { return components[i]; }

		// Actions
		void AddDevice(const char* port) 
		{ 
			components.push_back(new DeviceComponents(port)); 
			components[components.size() - 1]->id = components.size() - 1;
			components[components.size() - 1]->plotter->setID(components[components.size() - 1]->id);
		}
		void RemoveDevice(uint8_t i)
		{
			delete components[i];
			components.erase(components.begin() + i); 
		}

		void plotDevice(uint8_t id, RTPlot::Graphics* graphicsPtr)
		{
			if (components[id]->plotter->getPlotExitFlag()) 
				components[id]->plotter->plot(components[id]->serialDevice->getPort()->getName(), components[id]->getPlotFlagPtr(), graphicsPtr);
		}
		void plotDevices(RTPlot::Graphics* graphicsPtr)      { for (uint8_t i = 0; i < components.size(); i++) plotDevice(i, graphicsPtr); }
	};
}

#endif