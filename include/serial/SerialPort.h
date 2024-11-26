#ifndef _SERIALPORT__HPP_
#define _SERIALPORT__HPP_

#ifdef _WIN32
#include <Windows.h>
#else
#error This program is currently only supported in Windows
#endif

#include <iostream>
#include <vector>
#include <RTPlotVars.h>

namespace RTPlot
{
	class SerialPort
	{
		DCB          dcb = { 0 };	   // DCB == Device Control Block
		BYTE         byteSize;
		WORD         parity;
		bool         connected;
		bool		 verboseData;
		DWORD        errors;
		DWORD        baudRate;
		HANDLE       hCOM;
		COMSTAT      status;
		std::string  portName;
		COMMTIMEOUTS timeouts = { 0 }; // Serial reading timeouts

	public:
		SerialPort(void) = delete;
		SerialPort(const char* _port, DWORD _baudRate = CBR_9600, BYTE _byteSize = RTPLOT_MSG_SIZE, WORD _parity = NOPARITY, bool verboseData = false);
		~SerialPort(void);

		// Getters
		bool isConnected(void);
		const std::string& getName(void) { return portName; }

		// Setters
		void setName(const char* name);
		void setVerbose(bool vb) { verboseData = vb; }
		void setTimeouts(DWORD WriteTotalMultiplier = 10, DWORD ReadTotalMultiplier = 10, DWORD ReadInterval = 50, DWORD ReadTotalConstant = 1000, DWORD WriteTotalConstant = 1000);

		// Actions
		bool connect(void);
		void disconnect(void);
		bool clearBuffer(uint8_t flags = PURGE_RXCLEAR | PURGE_TXCLEAR);
		int8_t read(LPVOID buf, DWORD size);
		int8_t write(LPVOID buf, DWORD size);
		static std::vector<uint8_t> scanAvailablePorts(void);
	};
}


#endif