#ifndef _SERIALDEVICE__H_
#define _SERIALDEVICE__H_

#include <serial/SerialPort.h>
#include <mutex>

namespace RTPlot
{
	class SerialDevice
	{
		SerialPort*         port;

        const size_t        msgSize;
        void*               reading;
        std::vector<double> dReading;

        bool                verboseData = true;

	public:
        SerialDevice(void) = delete;
        SerialDevice(const char* portName, size_t size = RTPLOT_MSG_SIZE, uint32_t baudRate = CBR_115200);
        ~SerialDevice(void);

        // Getters
        std::vector<double>* GetMsgArray(void) { return &dReading; }

        bool IsConnected(void) { return port->IsConnected(); }
        SerialPort* GetPort(void) { return port; }

        // Setters
        void SetPortVerbose(bool vb) { verboseData = vb; port->SetVerbose(vb); }

        // Actions
        bool Recieve(uint32_t delay = RTPLOT_READING_DELAY);
        bool Send(const std::string& msg);
	};
}

#endif