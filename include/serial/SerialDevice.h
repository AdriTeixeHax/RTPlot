#ifndef _SERIALDEVICE__H_
#define _SERIALDEVICE__H_

#include "SerialPort.h"

#include <vector>

#define RTPLOT_FLOAT_STR_SIZE 7

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
        SerialDevice(const char* portName, size_t size = RTPLOT_BYTE_SIZE, uint32_t baudRate = CBR_115200) : port(new RTPlot::SerialPort(portName, baudRate)), msgSize(size), reading(new void*) { port->clearBuffer(); }
        ~SerialDevice(void) { delete[] reading; port->clearBuffer(); delete port; }

        // Getters
        double getMessage(void) { return dReading; }
        bool isConnected(void) { return port->isConnected(); }
        SerialPort* getPort(void) { return port; }

        // Setters
        bool changePort(const char* portName);
        void setVerbose(bool vb) { verboseData = vb; port->setVerbose(vb); }

        // Actions
        bool recieve(uint32_t delay = RTPLOT_READING_DELAY);
        static std::vector<uint8_t> scanSerialDevices(void);
	};
}

#endif