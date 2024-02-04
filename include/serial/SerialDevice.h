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
		RTPlot::SerialPort* port;

        const size_t msgSize;
        void*        reading;
        double       dReading = 0;

	public:
        SerialDevice(void) = delete;
        SerialDevice(const char* portName, size_t size = RTPLOT_BYTE_SIZE, uint32_t baudRate = CBR_115200) : port(new RTPlot::SerialPort(portName, baudRate)), msgSize(size), reading(new void*) { port->clearBuffer(); }
        ~SerialDevice(void) { delete[] reading; port->clearBuffer(); delete port; }

        bool recieve(bool verboseData = false, uint32_t delay = RTPLOT_READING_DELAY)
		{
            if (!port) { std::cerr << "[SerialDevice]: Error dereferencing port: it is nullptr." << std::endl; return false; }
            if (!port->isConnected()) { std::cout << "[SerialDevice]: Device not connected." << std::endl; return false; }

            int8_t readCode = port->read(reading, msgSize);
            if      (readCode == RTPLOT_ERROR) { std::cout << "[SerialDevice]: Could not read message from device." << std::endl; return false; }
            else if (readCode == RTPLOT_FINISHED)
            {
                if (reading == nullptr) { std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

                char tempMsg[RTPLOT_BYTE_SIZE];
                for (uint8_t i = 0; i < sizeof(tempMsg); i++)
                    tempMsg[i] = *((char*)reading + i);

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
                        if (rotationCounter > sizeof(tempMsg))
                        {
                            std::cout << "[SerialDevice]: Raw read data: " << tempMsg << std::endl;
                            std::cerr << "[SerialDevice]: Couldn't read a valid message. Clearing buffer..." << std::endl;
                            port->clearBuffer();
                            return false; 
                        }
                    }
                }

                if ((tempMsg[0] == ' ' || tempMsg[0] == '-') && tempMsg[7] == '\n')
                {
                    dReading = atof(tempMsg);
                    if (verboseData) std::cout << "[SerialDevice]: Converted read data: " << dReading << std::endl; 
                    Sleep(RTPLOT_READING_DELAY);
                }
            }

            return true;
		}

        bool changePort(const char* portName)
        {
            if (!port) return false;

            if (reading) delete[] reading;
            port->disconnect();

            reading = new void*;
            port->setName(portName);
            
            if (port->connect())
                return true;
            else return false;
        }

        double getMessage(void) { return dReading; }
        bool isConnected(void) { return port->isConnected(); }
	};
}

#endif