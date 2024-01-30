#ifndef _SERIALDEVICE__H_
#define _SERIALDEVICE__H_

#include "SerialPort.h"
#include <ctime>
#include <thread>

#define RTPLOT_FLOAT_STR_SIZE 7

namespace RTPlot
{
	class SerialDevice
	{
		RTPlot::SerialPort port;
        const size_t msgSize;

        void* reading;
        double dReading = 0;

	public:
        SerialDevice(const char* portName, size_t size = RTPLOT_BYTE_SIZE, uint32_t baudRate = CBR_115200) : port(portName, baudRate), msgSize(size), reading(new void*) { port.clearBuffer(); }
        ~SerialDevice(void) { delete[] reading; port.clearBuffer(); }

        bool recieve(bool verboseData = false, uint32_t delay = RTPLOT_READING_DELAY)
		{
            if (port.isConnected())
            {
                int8_t readCode = port.read(reading, msgSize);

                if (readCode == RTPLOT_FINISHED)
                {
                    if (reading == nullptr) { std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

                    char tempMsg[RTPLOT_BYTE_SIZE];
                    for (uint8_t i = 0; i < sizeof(tempMsg); i++)
                    {
                        tempMsg[i] = *((char*)reading + i);
                    }

                    if (tempMsg[7] != '\n') // Rotate buffer until tempMsg[7] == '\n'
                    {
                        uint8_t rotationCounter = 0;
                        while (tempMsg[7] != '\n')
                        {
                            char aux = tempMsg[0];
                            for (uint8_t i = 0; i < sizeof(tempMsg); i++)
                            {
                                if (i == sizeof(tempMsg) - 1) // Because it cannot copy the next element (goes out of scope)
                                {
                                    tempMsg[i] = aux;
                                    break;
                                }

                                tempMsg[i] = tempMsg[i + 1];
                            }

                            rotationCounter++;
                            if (rotationCounter > sizeof(tempMsg)) return false;
                        }
                    }

                    if ((tempMsg[0] == ' ' || tempMsg[0] == '-') && tempMsg[7] == '\n')
                    {
                        std::cout << "[SerialDevice]: ";

                        dReading = atof(tempMsg);
                        std::cout << dReading << std::endl;
                        Sleep(10);
                    }
                }
                else if (readCode == RTPLOT_ERROR)
                {
                    std::cout << "[SerialDevice]: Could not read message from device." << std::endl;
                    return false;
                }
            }
            else
            {
                std::cout << "[SerialDevice]: Device not connected." << std::endl;
                return false;
            }

            return true;
		}

        double getMessage(void) { return dReading; }
	};
}


#endif