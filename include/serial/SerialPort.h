#ifndef _SERIALPORT__HPP_
#define _SERIALPORT__HPP_

#ifdef _WIN32
	#include <Windows.h>
	#define USING_WINDOWS
#else
	#ifdef __linux__
		#include <libserial/SerialPort.h>
		#define USING_LINUX
	#endif
#endif

#include <iostream>
#include <vector>
#include <unistd.h>

#define RTPLOT_BYTE_SIZE 8
#define RTPLOT_INIT_WAIT_TIME 2000
#define RTPLOT_READING 1
#define RTPLOT_FINISHED 0
#define RTPLOT_ERROR -1

#define RTPLOT_READING_DELAY 0

#ifdef USING_WINDOWS
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
		SerialPort(const char* _port, DWORD _baudRate = CBR_115200, BYTE _byteSize = RTPLOT_BYTE_SIZE, WORD _parity = NOPARITY, bool verboseData = false);
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
		static std::vector<uint8_t> scanAvailablePorts(void);
	};
}
#endif

#ifdef USING_LINUX
namespace RTPlot
{
	class SerialPort
	{
		LibSerial::SerialPort serialPort;
		std::string  		  portName;
		uint32_t			  readingTimeout = 0;
		bool         		  connected;
		bool		 		  verboseData;

	public:
		SerialPort(void) = delete;
		SerialPort(const char* _port, LibSerial::BaudRate _baudRate, LibSerial::CharacterSize _byteSize = LibSerial::CharacterSize::CHAR_SIZE_8, LibSerial::Parity _parity = LibSerial::Parity::PARITY_NONE, bool verboseData = false);
		~SerialPort(void) { this->disconnect(); }

		// Getters
		const std::string& getName(void) const { return portName; }

		// Setters
		void setName(const char* name) { this->portName = name; }
		void setVerbose(bool vb) { verboseData = vb; }
		void setTimeouts(float timeout) { this->readingTimeout = timeout; }

		// Actions
		bool connect(void);
		void disconnect(void);
		bool isConnected(void);
		bool read(char& reading);
		static std::vector<std::string> scanAvailablePorts(void);
	};
}
#endif

#endif