#ifndef _SERIALDEVICE__H_
#define _SERIALDEVICE__H_

#include "SerialPort.h"

#include <vector>

namespace RTPlot
{
	class SerialDevice
	{
		SerialPort*  port;

        const size_t msgSize;
        void*        reading;
        double       dReading = 0;

        bool         verboseData = false;

	public:
        SerialDevice(void) = delete;
        SerialDevice(const char* portName, size_t size = RTPLOT_MSG_SIZE, uint32_t baudRate = CBR_115200) : port(new RTPlot::SerialPort(portName, baudRate)), msgSize(size), reading(new void*) { port->ClearBuffer(); }
        ~SerialDevice(void) { delete[] reading; port->ClearBuffer(); delete port; }

        // Getters
        double GetMsg(void) { return dReading; }
        bool IsConnected(void) { return port->IsConnected(); }
        SerialPort* GetPort(void) { return port; }

        // Setters
        bool ChangePort(const char* portName);
        void SetVerbose(bool vb) { verboseData = vb; port->SetVerbose(vb); }

        // Actions
        bool Recieve(uint32_t delay = RTPLOT_READING_DELAY);
        bool Send(const std::string& msg);
	};
}

#endif