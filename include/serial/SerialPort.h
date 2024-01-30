#ifndef _SERIALPORT__HPP_
#define _SERIALPORT__HPP_

#ifdef _WIN32
#include <Windows.h>
#else
#error This program is currently only supported in Windows
#endif

#include <iostream>

#define RTPLOT_BYTE_SIZE 8
#define RTPLOT_INIT_WAIT_TIME 2000
#define RTPLOT_READING 1
#define RTPLOT_FINISHED 0
#define RTPLOT_ERROR -1

#define RTPLOT_READING_DELAY 0

namespace RTPlot
{
	class SerialPort
	{
		const char* portName;
		HANDLE hCOM;
		COMSTAT status;
		DWORD errors;
		bool connected;

	public:
		SerialPort(void) = delete;
		SerialPort(const char* port, DWORD baudRate = CBR_115200, BYTE byteSize = RTPLOT_BYTE_SIZE, WORD parity = NOPARITY);
		~SerialPort(void);

		bool isConnected(void)
		{
			DWORD modemStatus = 0;
			if (!GetCommModemStatus(hCOM, &modemStatus))
			{
				DWORD error = GetLastError();
				std::cerr << "[SerialPort]: Error getting modem status. Error code: " << error << std::endl;
				return false;
			}

			return connected;
		}

		int8_t read(LPVOID buf, DWORD size);

		bool clearBuffer(void) { return PurgeComm(hCOM, PURGE_RXCLEAR | PURGE_TXCLEAR); }
	};
}


#endif