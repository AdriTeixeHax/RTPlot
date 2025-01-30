#include <thread>
#include <mutex>

#include <serial/SerialDevice.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	class DeviceComponent
	{
		std::thread   thread;
		RealTimePlot  plotter;
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
		bool        GetPlotExitFlag(uint8_t i)  { return plotter.GetPlotExitFlag(); }
		uint8_t     GetID          (void)       { return id; }
		SerialPort* GetPort        (void)       { return serialDevice->GetPort(); }
		std::string GetPortName    (void) const { return serialDevice->GetPort()->GetName(); }
		std::string GetPortNameGUI (void) const;
		bool		GetKillFlag    (void) const { return killFlag; }

		// Setters
		void SetID(uint8_t id)                               { this->id = id; }

		// Functions
		int8_t Plot(const std::string& portName) { return plotter.Plot(portName, &killFlag); }

		// Thread functions
		void SerialReadingFunc(void);
	};
}