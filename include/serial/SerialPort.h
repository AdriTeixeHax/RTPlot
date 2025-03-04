/// SerialPort.h - Header file for the SerialPort class, used to manage serial communication between the computer and a microcontroller via COM ports.

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

constexpr const char* LONG_COM_PORT_PREFIX = "\\\\.\\";

namespace RTPlot
{
	class SerialPort
	{
		DCB          dcb = { 0 };	   // DCB == Device Control Block
		BYTE         byteSize;		   // Byte size
		WORD         parity;		   // Parity type
		bool         connected;		   // Connection status
		bool		 verboseData;	   // Send messages or not through console
		uint8_t		 readingDelay = 1; // Delay to allow status update
		DWORD        errors;		   // Error messages
		DWORD        baudRate;		   // Baud rate (CBR_XXXXXX)
		HANDLE       hCOM;			   // Handle variable for the COM port
		COMSTAT      status;		   // Status of the COM port
		std::string  portName;         // Port name in standard short ASCII format
		COMMTIMEOUTS timeouts = { 0 }; // Serial reading timeouts

	public:
		SerialPort(void) = delete;
		SerialPort(const std::string& _port, DWORD _baudRate = CBR_115200, BYTE _byteSize = 8, WORD _parity = NOPARITY, bool _verboseData = true);
		~SerialPort(void);

		// Getters
		bool			   IsConnected(void) const;
		const std::string& GetName    (void) const { return portName; }
		const std::string& GetNameStr (void) const;

		// Setters
		void SetName(const std::string& name) { this->portName = name; }
		void SetVerbose(bool vb) { verboseData = vb; }
		void SetTimeouts(DWORD WriteTotalMultiplier = 10, DWORD ReadTotalMultiplier = 10, DWORD ReadInterval = 50, DWORD ReadTotalConstant = 1000, DWORD WriteTotalConstant = 1000);

		// Actions
		bool   Connect(void);
		bool   Disconnect(void);
		bool   ClearBuffer(uint8_t flags = PURGE_RXCLEAR | PURGE_TXCLEAR);
		int8_t Read (LPVOID buf, DWORD size);
		int8_t Write(LPVOID buf, DWORD size);

		// Static functions
		static std::vector<uint8_t> ScanAvailablePorts(void);
	};
}


#endif