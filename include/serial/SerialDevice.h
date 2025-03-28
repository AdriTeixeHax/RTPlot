/// SerialDevice.h - Header file for the SerialDevice class, used to manage information gathered from a SerialPort object.

#ifndef _SERIALDEVICE__H_
#define _SERIALDEVICE__H_

#include <serial/SerialPort.h>

namespace RTPlot
{
	class SerialDevice
	{
		SerialPort*         port;
        char                readingRaw[RTPLOT_MSG_SIZE];
        std::vector<double> readingVals;
        bool                verboseData = true;

	public:
        SerialDevice(void) = delete;
        SerialDevice(const char* portName, size_t size = RTPLOT_MSG_SIZE, uint32_t baudRate = CBR_115200);
        ~SerialDevice(void);

        // Getters
        const std::vector<double>& GetReadingVals     (void)    { return  readingVals; }
        std::vector<double>*       GetReadingPtr      (void)    { return &readingVals; }
        SerialPort*                GetPort            (void)    { return port; }
        bool                       IsConnected        (void)    { return port->IsConnected(); }
        const std::string&         GetFriendlyPortName(void)    { return port->GetFriendlyName(); }

        // Setters
        void                       SetVerbose         (bool vb) { this->verboseData = vb; }
        void                       SetPortVerbose     (bool vb) { verboseData = vb; port->SetVerbose(vb); }

        // Actions
        bool                       Recieve            (void);
        bool                       Send               (const char* msg, uint32_t len);
        int8_t                     ProcessData        (void);
        void                       PrintData          (void);

        // JSON managing
        JSON                       toJSON(void);
        void                       fromJSON(const JSON& j);
	};
}

#endif