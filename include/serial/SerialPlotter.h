/// SerialPlotter.h - Header file for the SerialPlotter class, used to manage information gathered from a SerialDevice object and plot it in real-time.

#include <thread>
#include <mutex>

#include <serial/SerialDevice.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	class SerialPlotter
	{
		// Enable/disable, add/remove
		bool		  killFlag       = true;			// Passed to the RealTimePlot. If the window is closed (killFlag == false), the object destroys itself.
		bool		  addVariable	 = false;			// If true, a new variable is added to the plot.
		bool		  removeVariable = false;			// If true, a variable is removed from the plot.	
		uint32_t      varToRemove	 = 0U;				// Index of the variable to remove.

		// Serial communication
		SerialDevice* serialDevice;						// Serial device object
		std::thread   serialCommThread;					// Thread to handle serial communication
		bool          exitCommThreadFlag = false;		// Flag to exit the serial communication thread
		char		  commandToSend[RTPLOT_MSG_SIZE];	// Command to send to the device
		bool		  sendCommand		 = false;		// Flag to send a command to the device

		// Plotting
		RealTimePlot* plotter;							// Real-time plot object

		// Mutex
		std::mutex mutex;

	public:
		SerialPlotter(const char* port);
		~SerialPlotter(void);

		// Getters
		bool        GetPlotExitFlag(uint8_t i)  { return plotter->GetPlotExitFlag(); }
		SerialPort* GetPort        (void)       { return serialDevice->GetPort(); }
		std::string GetPortName    (void) const { return serialDevice->GetPort()->GetName(); }
		bool		GetKillFlag    (void) const { return killFlag; }

		// Functions
		void Plot(const std::string& portName);

		// Thread functions
		void SerialFunc(void);
	};
}