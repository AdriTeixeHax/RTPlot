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
		RealTimePlot* plotter;
		SerialDevice* serialDevice;
		bool          exitThreadFlag = false;
		bool		  sendMsgFlag = false;
		uint8_t       id = 0;
		double        reading = 0;

	public:
		DeviceComponent(const char* port, Graphics* graphicsPtr);
		~DeviceComponent(void);

		// Getters
		bool        GetPlotExitFlag(uint8_t i)  { return plotter->GetPlotExitFlag(); }
		uint8_t     GetID          (void)       { return id; }
		SerialPort* GetPort        (void)       { return serialDevice->GetPort(); }
		std::string GetPortName    (void) const { return serialDevice->GetPort()->GetName(); }

		// Setters
		void SetID(uint8_t id) { this->id = id; }
		void SetPlotterID(uint8_t newID) { plotter->SetID(newID); }

		// Functions
		int8_t Plot(const std::string& portName) { return plotter->Plot(portName); }

		// Thread functions
		void SerialReadingFunc(void);
	};
}