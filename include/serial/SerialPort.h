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
		DCB          dcb = { 0 };	   // DCB == Device Control Block
		BYTE         byteSize;
		WORD         parity;
		bool         connected;
		DWORD        errors;
		DWORD        baudRate;
		HANDLE       hCOM;
		COMSTAT      status;
		const char*  portName;
		COMMTIMEOUTS timeouts = { 0 }; // Serial reading timeouts

	public:
		SerialPort(void) = delete;
		SerialPort(const char* _port, DWORD _baudRate = CBR_115200, BYTE _byteSize = RTPLOT_BYTE_SIZE, WORD _parity = NOPARITY);
		~SerialPort(void);

		// Getters
		bool isConnected(void);

		// Setters
		void setName(const char* name);

		// Actions
		bool connect(void);
		void disconnect(void);
		bool clearBuffer(uint8_t flags = PURGE_RXCLEAR | PURGE_TXCLEAR);
		int8_t read(LPVOID buf, DWORD size);
	};
}


#endif