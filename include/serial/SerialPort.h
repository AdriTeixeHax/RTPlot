/// SerialPort.h - Header file for the SerialPort class, used to manage serial communication between the computer and a microcontroller via COM ports.

#ifndef _SERIALPORT__HPP_
#define _SERIALPORT__HPP_

#ifdef _WIN32
	#include <Windows.h>
	#include <devguid.h>
	#include <regstr.h>
	#include <setupapi.h>
	#pragma comment(lib, "Setupapi.lib")
#endif
#ifdef __linux__
	#include <libserialport.h>
	#include <unistd.h>
	#include <chrono>

	using millis_t = std::chrono::milliseconds;
#endif

#include <iostream>
#include <vector>
#include <string>

#include <RTPlotFunctions.h>
#include <RTPlotVars.h>

#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

#ifdef _WIN32
namespace RTPlot
{
	class SerialPort
	{
		DCB                dcb = { 0 };	     // DCB == Device Control Block
		BYTE               byteSize;		 // Byte size
		WORD               parity;		     // Parity type
		bool               connected;		 // Connection status
		bool		       verboseData;	     // Send messages or not through console
		uint8_t		       readingDelay = 5; // Delay to allow status update
		DWORD              errors;		     // Error messages
		DWORD              baudRate;		 // Baud rate (CBR_XXXXXX)
		HANDLE             hCOM;			 // Handle variable for the COM port
		COMSTAT            status;			 // Status of the COM port
		std::string        portName;         // Port name in standard short ASCII format
		COMMTIMEOUTS       timeouts = { 0 }; // Serial reading timeouts
		std::string        friendlyName;	 // Friendly name of the COM port

	public:
		SerialPort(void) = delete;
		SerialPort(const std::string& _port, DWORD _baudRate = CBR_115200, BYTE _byteSize = 8, WORD _parity = NOPARITY, bool _verboseData = true);
		~SerialPort(void);

		// Getters
		bool			   IsConnected    (void) const;
		const std::string& GetFriendlyName(void) const              { return friendlyName;}
		const std::string& GetName        (void) const              { return portName; }
		uint8_t			   GetReadingDelay(void) const				{ return readingDelay; }
		COMMTIMEOUTS	   GetTimeouts	  (void) const				{ return timeouts; }

		// Setters
		void               SetName        (const std::string& name) { this->portName = name; }
		void               SetVerbose     (bool vb)                 { verboseData = vb; }
		void               SetReadingDelay(uint8_t delay)           { readingDelay = delay; }
		void               SetTimeouts    (DWORD WriteTotalMultiplier = 10, DWORD ReadTotalMultiplier = 10, DWORD ReadInterval = 50, DWORD ReadTotalConstant = 1000, DWORD WriteTotalConstant = 1000);

		// Actions
		bool               Connect        (void);
		bool               Disconnect     (void);
		void               CalcFrndlyName (void);
		bool               ClearBuffer    (uint8_t flags = PURGE_RXCLEAR | PURGE_TXCLEAR);
		int8_t             Read           (LPVOID buf, DWORD size);
		int8_t             Write          (LPVOID buf, DWORD size);

		// JSON managing
		JSON			   toJSON		  (void);
		void			   fromJSON		  (const JSON& j);

		// Static functions
		static std::vector<uint8_t> ScanAvailablePorts(void);
	};
}
#endif

#ifdef __linux__

namespace RTPlot
{
	class SerialPort
	{
		struct sp_port* port;
		bool            connected;
		bool		    verboseData;
		uint8_t		    readingDelay = 5;
		uint32_t        byteSize;
		uint32_t        baudRate;
		sp_parity       parity;
		std::string     portName;
		std::string     friendlyName;

	public:
		SerialPort(void) = delete;
		SerialPort(const std::string& _portName, uint32_t _baudRate = 115200, uint32_t _byteSize = 8, sp_parity _parity = SP_PARITY_NONE, bool _verboseData = true);
		~SerialPort(void);

		// Getters
		bool			   IsConnected    (void) const;
		const std::string& GetFriendlyName(void) const              { return friendlyName;}
		const std::string& GetName        (void) const              { return portName; }
		uint8_t			   GetReadingDelay(void) const				{ return readingDelay; }

		// Setters
		void               SetName        (const std::string& name) { this->portName = name; }
		void               SetVerbose     (bool vb)                 { verboseData = vb; }
		void               SetReadingDelay(uint8_t delay)           { readingDelay = delay; }

		// Actions
		bool               Connect        (void);
		bool               Disconnect     (void);
		void			   ClearBuffer    (sp_buffer whichBuffer = SP_BUF_BOTH);
		void               CalcFrndlyName (void);
		int8_t             Read           (void* buf, size_t size);
		int8_t             Write          (void* buf, size_t size);

		// JSON managing
		JSON			   toJSON		  (void);
		void			   fromJSON		  (const JSON& j);

		// Static functions
		static std::vector<std::string> ScanAvailablePorts(void);
	};
}
#endif


#endif