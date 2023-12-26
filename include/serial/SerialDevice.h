#ifndef _SERIALDEVICE__H_
#define _SERIALDEVICE__H_

#include "SerialPort.h"

namespace RTPlot
{
	class SerialDevice
	{
		SerialPort port;
		char msg[RTPLOT_BYTE_SIZE];

	public:
		SerialDevice(const char* portName) : port(portName) { }

        void recieve(uint8_t msgSize, bool plot = false, uint32_t delay = RTPLOT_READING_DELAY)
		{
            if (port.isConnected())
            {
                static clock_t endTime = clock();

                int8_t readCode = port.read(msg, RTPLOT_BYTE_SIZE);
                if (readCode == RTPLOT_FINISHED && msg[RTPLOT_BYTE_SIZE - 1] == '\n')
                {
                    if (clock() - endTime > delay)
                    {
                        if (plot) for (int i = 0; i < RTPLOT_BYTE_SIZE; i++) std::cout << msg[i];

                        endTime = clock();
                    }
                }
                else if (readCode == RTPLOT_ERROR) std::cout << "[SerialDevice]: Could not read message from device. Read code: " << readCode << std::endl;
            }
            else std::cout << "[SerialDevice]: Device not connected." << std::endl;
		}

        char* getMessage(void) { return msg; }
	};
}


#endif