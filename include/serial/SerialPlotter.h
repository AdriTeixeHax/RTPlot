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
		bool		  killFlag           = true;		// Passed to the RealTimePlot. If the window is closed (killFlag == false), the object destroys itself.
		bool		  addVariable	     = false;		// If true, a new variable is added to the plot.
		bool		  removeVariable     = false;		// If true, a variable is removed from the plot.	
		uint32_t      varToRemove	     = 0U;			// Index of the variable to remove.

		// Serial communication
		SerialDevice* serialDevice;						// Serial device object
		std::thread   serialCommThread;					// Thread to handle serial communication
		bool          exitCommThreadFlag = false;		// Flag to exit the serial communication thread
		bool		  sendCommand		 = false;		// Flag to send a command to the device
		char		  commandToSend[RTPLOT_MSG_SIZE];	// Command to send to the device

		// Plotting
		RealTimePlot* realTimePlotter;					// Real-time plot object

		// Mutex
		std::mutex    mutex;

		// Log message pointer
		std::string*  logMsgPtr;

	public:
		SerialPlotter (const char* _port, std::string* _logMsg);
		SerialPlotter (const SerialPlotter& s);
		~SerialPlotter(void);

		// Getters
		bool          GetPlotExitFlag    (uint8_t i)     { return realTimePlotter->GetPlotExitFlag(); }
		SerialPort*   GetPort            (void)          { return serialDevice->GetPort(); }
		std::string   GetPortName        (void) const    { return serialDevice->GetPort()->GetName(); }
		bool		  GetKillFlag        (void) const    { return killFlag; }
		SerialDevice* GetSerialDevice    (void)          { return serialDevice; }
		RealTimePlot* GetRealTimePlotter (void)		     { return realTimePlotter; }

		// Setters
		void          SetReadingDelay    (uint8_t delay) { serialDevice->GetPort()->SetReadingDelay(delay); }

		// Functions
		void          Plot               (const std::string& portName);
		void          SerialOptionsWindow(bool* serialOptionsFlag, std::string* logMsg);

		// Thread functions
		void          SerialFunc         (void);

		// JSON managing
		JSON          toJSON             (void);
		void          fromJSON           (const JSON& j);
		void		  SaveConfig		 (const std::string& filepath);
		void		  LoadConfig		 (const std::string& filepath);
	};
}