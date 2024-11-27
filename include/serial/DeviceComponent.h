#include <thread>
#include <mutex>

#include <serial/SerialDevice.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	class DeviceComponent
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

		DeviceComponent(const char* port, Graphics* graphicsPtr);
		~DeviceComponent(void);

		bool  GetPlotFlag(void) const { return plotFlag; }
		bool* GetPlotFlagPtr(void) { if (&plotFlag) return &plotFlag; }
		void  SetPlotFlag(bool state) { plotFlag = state; }

		void SerialReadingFunc(void);
	};
}