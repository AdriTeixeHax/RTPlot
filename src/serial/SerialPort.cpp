#include <serial/SerialPort.h>

#include <string>

namespace RTPlot
{
	SerialPort::SerialPort(const char* _port, DWORD _baudRate, BYTE _byteSize, WORD _parity, bool verboseData) : portName(_port), baudRate(_baudRate), parity(_parity), hCOM((void*)0), status({ 0 }), errors(0), connected(false), byteSize(_byteSize)
	{
		this->Connect();
		this->SetTimeouts();
	}

	SerialPort::~SerialPort(void) { this->Disconnect(); }

	void SerialPort::SetName(const char* name)
	{
		this->portName = name;
	}

	void SerialPort::SetTimeouts(DWORD WriteTotalMultiplier, DWORD ReadTotalMultiplier, DWORD ReadInterval, DWORD ReadTotalConstant, DWORD WriteTotalConstant)
	{
		timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
		timeouts.ReadTotalTimeoutMultiplier  = ReadTotalMultiplier;
		timeouts.ReadIntervalTimeout         = ReadInterval;
		timeouts.ReadTotalTimeoutConstant    = ReadTotalConstant;
		timeouts.WriteTotalTimeoutConstant   = WriteTotalConstant;
	}

	bool SerialPort::Connect(void)
	{
		hCOM = CreateFileA(static_cast<LPCSTR>(portName.c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);

		switch (GetLastError())
		{
		case ERROR_FILE_NOT_FOUND:
			if (verboseData) std::cerr << "[SerialPort]: Device not connected at port " << portName << "." << std::endl;
			return false;

		case ERROR_ACCESS_DENIED:
			if (verboseData) std::cerr << "[SerialPort]: Device being used by another process at port " << portName << "." << std::endl;
			return false;

		case NO_ERROR:
			if (!GetCommState(hCOM, &dcb)) { if (verboseData) std::cout << "[SerialPort]: Failed to get COM port parameters." << std::endl; return false; }

			dcb.BaudRate = baudRate;
			dcb.ByteSize = byteSize;
			dcb.Parity = parity;
			dcb.StopBits = ONESTOPBIT;
			dcb.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(hCOM, &dcb)) { if (verboseData) std::cerr << "[SerialPort]: Failed to set COM port parameters." << std::endl; return false; }

			if (!SetCommTimeouts(hCOM, &timeouts)) { if (verboseData) std::cerr << "[SerialPort]: Failed to set COM port timeouts." << std::endl; return false; }

			// Everything OK
			PurgeComm(hCOM, PURGE_RXCLEAR | PURGE_TXCLEAR); // Clear buffer
			if (verboseData) std::cout << "[SerialPort]: Connected to device at COM port " << portName << "." << std::endl;
			connected = true;
			return true;

		default:
			return false;
		}
	}

	void SerialPort::Disconnect(void)
	{
		if (connected == true)
		{
			connected = false;
			CloseHandle(hCOM);
			hCOM = (void*)0;
		}
	}

	bool SerialPort::ClearBuffer(uint8_t flags)
	{
		if (PurgeComm(hCOM, flags))
		{
			if (verboseData) std::cout << "[SerialPort]: Buffer cleared." << std::endl;
			return true;
		}
		else
		{
			if (verboseData) std::cerr << "[SerialPort]: Failed to clear buffer." << std::endl;
			return false;
		}
	}

	bool SerialPort::IsConnected(void)
	{
		DWORD modemStatus = 0;
		if (!GetCommModemStatus(hCOM, &modemStatus))
		{
			DWORD error = GetLastError();
			if (verboseData) std::cerr << "[SerialPort]: Error getting modem status. Error code : " << error << std::endl;
			return false;
		}

		return connected;
	}

	int8_t SerialPort::Read(LPVOID buf, DWORD size)
	{
		DWORD bytesRead;
		DWORD bytesToRead = 0;

		ClearCommError(hCOM, &errors, &status);

		if (status.cbInQue > 0)
		{
			if (status.cbInQue >= size) bytesToRead = size;
			else bytesToRead = status.cbInQue;
		}

		static uint8_t readingCount = 0;
		if (readingCount >= 5)
		{
			ClearBuffer(PURGE_RXCLEAR);
			readingCount = 0;
		}

		if (ReadFile(hCOM, buf, bytesToRead, &bytesRead, NULL))
		{
			if (bytesRead == bytesToRead) { return RTPLOT_FINISHED; readingCount++; }
			else if (bytesRead < bytesToRead) return RTPLOT_READING;
			else if (bytesRead > bytesToRead) return RTPLOT_ERROR;
		}
		else
		{
			if (verboseData) std::cerr << "[SerialPort]: Error reading from COM port handler. Error: " << GetLastError() << std::endl;
			if (GetLastError() == 0x3E6) std::cerr << "[SerialPort]: Invalid access to memory location." << std::endl;
			return RTPLOT_ERROR;
		}
	}

	int8_t SerialPort::Write(LPVOID buf, DWORD size)
	{
		// Write data to the serial port
		const char* data = "Hello, Serial Port!\n";
		DWORD bytesWritten;
		if (!WriteFile(hCOM, data, strlen(data), &bytesWritten, NULL))
		{
			if (verboseData) std::cerr << "Error writing to serial port!" << std::endl;
			return RTPLOT_ERROR;
		}

		if (verboseData) std::cout << "Data written to serial port successfully!" << std::endl;

		return RTPLOT_FINISHED;
	}

	std::vector<uint8_t> SerialPort::ScanAvailablePorts(void)
	{
		std::vector<uint8_t> ports;
		static std::vector<std::wstring> portNames;

		if (portNames.size() < 1) // Empty
		{
			for (uint8_t portNumber = 0; portNumber < 255U; portNumber++)
			{
				portNames.push_back(L"COM" + std::to_wstring(portNumber));
			}
		}

		for (uint8_t portNumber = 0; portNumber < RTPLOT_MAX_PORT_NUMBER; portNumber++)
		{
			HANDLE hPort = CreateFileW(portNames[portNumber].c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hPort != INVALID_HANDLE_VALUE)
			{
				ports.push_back(portNumber);
			}
			CloseHandle(hPort);
		}

		return ports;
	}
}
