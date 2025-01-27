#include <thread>
#include <mutex>

#include <serial/SerialDevice.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	class DeviceComponent
	{
		std::thread   thread;
		std::mutex    mutex;
		std::vector<RealTimePlot*> plotters;
		SerialDevice* serialDevice;
		bool          exitThreadFlag = false;
		bool		  sendMsgFlag = false;
		uint8_t       id = 0;
		double        reading = 0;
		Graphics*     graphicsPtr;

		bool		  killFlag = true;

	public:
		DeviceComponent(const char* port, Graphics* graphicsPtr);
		~DeviceComponent(void);

		// Getters
		bool        GetPlotExitFlag(uint8_t i)  { return plotters.at(i)->GetPlotExitFlag(); }
		uint8_t     GetID          (void)       { return id; }
		SerialPort* GetPort        (void)       { return serialDevice->GetPort(); }
		std::string GetPortName    (void) const { return serialDevice->GetPort()->GetName(); }
		std::string GetPortNameGUI (void) const;
		size_t      GetPlottersSize(void) const { return plotters.size(); }
		bool		GetKillFlag    (void) const { return killFlag; }

		// Setters
		void SetID(uint8_t id)                               { this->id = id; }
		void SetPlotterID(uint8_t plotterNum, uint8_t newID) { plotters.at(plotterNum)->SetID(newID); }
		void AddPlotter(void)                                { plotters.push_back(new RealTimePlot(graphicsPtr)); }
		void DeletePlotter(uint8_t plotterID)                { plotters.erase(plotters.begin() + plotterID); }

		// Functions
		int8_t Plot(uint8_t id, const std::string& portName) { return plotters.at(id)->Plot(portName, &killFlag); }

		// Thread functions
		void SerialReadingFunc(void);
	};
}