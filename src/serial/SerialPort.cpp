#include <serial/SerialPort.h>

namespace RTPlot
{
	SerialPort::SerialPort(const std::string& _port, DWORD _baudRate, BYTE _byteSize, WORD _parity, bool _verboseData) : 
		byteSize(_byteSize),
		parity(_parity), 
		connected(false),
		verboseData(_verboseData),
		errors(0), 
		baudRate(_baudRate), 
		hCOM((void*)0), 
		status({ 0 }), 
		portName(_port)
	{
		if (this->Connect()) this->SetTimeouts();
		else std::cerr << "[SerialPort]: Error constructing SerialPort object because couldn't connect with the serial port." << std::endl;
	}

	SerialPort::~SerialPort(void) 
	{ 
		while (!this->Disconnect());
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
		if (portName.rfind("\\\\.\\", 0) != 0) { std::cerr << "[SerialPort]: Couldn't connect. Invalid port name." << std::endl; }

		hCOM = CreateFileA(reinterpret_cast<LPCSTR>(portName.c_str()), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (!hCOM) if (verboseData) { std::cerr << "[SerialPort]: Invalid handle pointer." << std::endl; return false; }
		
		static DWORD errorCode = 0;
		static DWORD errorCodePrev = 0;

		errorCode = GetLastError();

		switch (errorCode)
		{
		case ERROR_FILE_NOT_FOUND:
			if (verboseData && errorCode != errorCodePrev) std::cerr << "[SerialPort]: Device not connected at port " << portName.c_str() << "." << std::endl;
			return false;

		case ERROR_ACCESS_DENIED:
			if (verboseData && errorCode != errorCodePrev) std::cerr << "[SerialPort]: Device being used by another process at port " << portName.c_str() << "." << std::endl;
			return false;

		case NO_ERROR:
			if (!GetCommState(hCOM, &dcb)) { if (verboseData && errorCode != errorCodePrev) std::cout << "[SerialPort]: Failed to get COM port parameters." << std::endl; return false; }

			dcb.BaudRate = baudRate;
			dcb.ByteSize = byteSize;
			dcb.Parity = parity;
			dcb.StopBits = ONESTOPBIT;
			dcb.fDtrControl = DTR_CONTROL_ENABLE;

			if (!SetCommState(hCOM, &dcb)) { if (verboseData && errorCode != errorCodePrev) std::cerr << "[SerialPort]: Failed to set COM port parameters." << std::endl; return false; }

			if (!SetCommTimeouts(hCOM, &timeouts)) { if (verboseData && errorCode != errorCodePrev) std::cerr << "[SerialPort]: Failed to set COM port timeouts." << std::endl; return false; }

			// Everything OK
			PurgeComm(hCOM, PURGE_RXCLEAR | PURGE_TXCLEAR); // Clear buffer
			if (verboseData && errorCode != errorCodePrev) std::cout << "[SerialPort]: Connected to device at COM port " << portName.c_str() << "." << std::endl;
			connected = true;
			return true;

		default:
			return false;
		}

		errorCodePrev = errorCode;
	}

	bool SerialPort::Disconnect(void)
	{
		if (connected == true)
		{
			this->ClearBuffer();
			if (hCOM == INVALID_HANDLE_VALUE) std::cerr << "[SerialPort]: Port handle is invalid." << std::endl;
			if (!CloseHandle(hCOM)) 
			{
				std::cerr << "[SerialPort]: Couldn't close the serial port. Error " << GetLastError() << std::endl;
				return false;
			}
			connected = false;
			hCOM = INVALID_HANDLE_VALUE;
			return true;
		}
		else std::cerr << "[SerialPort]: The device trying to disconnect is not connected." << std::endl;
		return false;
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

	bool SerialPort::IsConnected(void) const
	{
		DWORD modemStatus = 0;

		// Error checking.
		if (!hCOM || hCOM == 0) { if (verboseData) std::cerr << "[SerialPort]: invalid handle." << std::endl; return false; }
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
		if (!hCOM) { std::cerr << "[SerialPort]: invalid handle value." << std::endl; return -1; }

		DWORD bytesRead = 0;
		DWORD bytesToRead = 0;
		static uint8_t readingCount = 0;

		// Check COM status
		Sleep(readingDelay); // Small delay to allow status update ¡IMPORTANT!
		ClearCommError(hCOM, &errors, &status);

		if (status.cbInQue > 0) bytesToRead = (status.cbInQue >= size) ? size : status.cbInQue;

		if (bytesToRead == 0) return RTPLOT_READING; // Avoid zero-byte reads

		// If repeated reading failures, clear the buffer
		if (readingCount >= 5)
		{
			ClearBuffer(PURGE_RXCLEAR);
			readingCount = 0;
		}

		if (ReadFile(hCOM, buf, bytesToRead, &bytesRead, NULL))
		{
			if (bytesRead == bytesToRead) { readingCount = 0; return RTPLOT_FINISHED; }
			else return RTPLOT_READING;
		}
		else
		{
			readingCount++;
			if (verboseData) std::cerr << "[SerialPort]: Error reading from COM port. Error: " << GetLastError() << std::endl;
			return RTPLOT_ERROR;
		}
	}


	int8_t SerialPort::Write(LPVOID buf, DWORD size)
	{
		DWORD bytesWritten;
		if (!WriteFile(hCOM, buf, size, &bytesWritten, NULL))
		{
			if (verboseData) std::cerr << "Error writing to serial port!" << std::endl;
			return RTPLOT_ERROR;
		}

		if (verboseData) std::cout << "Data written to serial port successfully!" << std::endl;

		return RTPLOT_FINISHED;
	}

	std::vector<uint8_t> SerialPort::ScanAvailablePorts(void)
	{
		static std::vector<std::wstring> portNames;
		if (portNames.size() < 1) // Create names just once
		{
			for (uint8_t portNumber = 0; portNumber < RTPLOT_MAX_PORT_NUMBER; portNumber++)
			{
				portNames.push_back(L"\\\\.\\COM" + std::to_wstring(portNumber));
			}
		}

		std::vector<uint8_t> ports;
		for (uint8_t portNumber = 0; portNumber < RTPLOT_MAX_PORT_NUMBER; portNumber++)
		{
			HANDLE hPort = CreateFileW(portNames[portNumber].c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
			if (hPort != INVALID_HANDLE_VALUE)
			{
				ports.push_back(portNumber);
			}
			CloseHandle(hPort);
		}

		return ports;
	}
}