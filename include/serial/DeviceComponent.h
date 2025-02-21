#include <thread>
#include <semaphore>

#include <serial/SerialDevice.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	class DeviceComponent
	{
		std::thread   thread;
		RealTimePlot  plotter;
		SerialDevice* serialDevice;
		bool          exitThreadFlag	= false;
		bool		  sendMsgFlag		= false;
		double        reading			= 0;
		Graphics*     graphicsPtr;
		char		  command[RTPLOT_MSG_SIZE];
		bool		  sendCommand		= false;
		bool		  addVariable		= false;
		bool		  removeVariable	= false;
		uint32_t      varToRemove		= 0;

		bool		  killFlag			= true;

	public:
		DeviceComponent(const char* port, Graphics* graphicsPtr);
		~DeviceComponent(void);

		// Getters
		bool        GetPlotExitFlag(uint8_t i)  { return plotter.GetPlotExitFlag(); }
		SerialPort* GetPort        (void)       { return serialDevice->GetPort(); }
		std::string GetPortName    (void) const { return serialDevice->GetPort()->GetName(); }
		std::string GetPortNameGUI (void) const;
		bool		GetKillFlag    (void) const { return killFlag; }

		// Functions
		void Plot(const std::string& portName);

		// Thread functions
		void SerialFunc(void);
	};
}