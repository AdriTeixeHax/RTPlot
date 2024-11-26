#include <thread>
#include <mutex>

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

		DeviceComponents(const char* port);
		~DeviceComponents(void);

		bool  GetPlotFlag(void) { return plotFlag; }
		bool* GetPlotFlagPtr(void) { if (&plotFlag) return &plotFlag; }
		void  SetPlotFlag(bool state) { plotFlag = state; }

		void SerialReadingFunc(void);
	};
}