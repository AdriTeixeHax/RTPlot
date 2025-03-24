#include <serial/SerialPort.h>

#ifdef _WIN32
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
		portName(_port),
		friendlyName("")
	{
		this->SetTimeouts();
		if (this->Connect()) this->CalcFrndlyName();
		else std::cerr << "[SerialPort]: Error constructing SerialPort object because couldn't connect with the serial port." << std::endl;
	}

	SerialPort::~SerialPort(void) 
	{ 
		this->Disconnect();
	}

	void SerialPort::CalcFrndlyName(void)
	{
		HDEVINFO hDevInfo;
		SP_DEVINFO_DATA DeviceInfoData;
		char deviceName[256] = { 0 };
		DWORD size = 0;

		// Get device information set for ports
		hDevInfo = SetupDiGetClassDevs(&GUID_DEVCLASS_PORTS, 0, 0, DIGCF_PRESENT);
		if (hDevInfo == INVALID_HANDLE_VALUE) std::cerr << "[SerialPort]: Unable to get device list to get friendly serial port name." << std::endl;

		// Enumerate devices
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (DWORD i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
		{
			// Get the COM port name from registry
			if (SetupDiGetDeviceRegistryPropertyA(hDevInfo, &DeviceInfoData, SPDRP_FRIENDLYNAME, nullptr,
				(PBYTE)deviceName, sizeof(deviceName), &size))
			{
				std::string tempFriendlyName = deviceName;

				// Check if the friendly name contains the target COM port
				if (tempFriendlyName.find(StripPortNamePrefix(this->portName)) != std::string::npos)
				{
					SetupDiDestroyDeviceInfoList(hDevInfo);
					this->friendlyName = tempFriendlyName;
				}
			}
		}

		// Cleanup
		SetupDiDestroyDeviceInfoList(hDevInfo);
		std::cerr << "[SerialPort]: COM port not found when getting friendly serial port name." << std::endl;
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
		Sleep(readingDelay); // Small delay to allow status update �IMPORTANT!
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

	std::vector<std::string> SerialPort::ScanAvailablePorts(void)
	{
		static std::vector<std::wstring> portNames;
		if (portNames.size() < 1) // Create names just once
		{
			for (uint8_t portNumber = 0; portNumber < RTPLOT_MAX_PORT_NUMBER; portNumber++)
			{
				portNames.push_back(L"\\\\.\\COM" + std::to_wstring(portNumber));
			}
		}

		std::vector<std::string> ports;
		for (uint8_t portNumber = 0; portNumber < RTPLOT_MAX_PORT_NUMBER; portNumber++)
		{
			HANDLE hPort = CreateFileW(portNames[portNumber].c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
			if (hPort != INVALID_HANDLE_VALUE)
			{
				ports.push_back("\\\\.\\COM" + std::to_string(portNumber));
			}
			CloseHandle(hPort);
		}

		return ports;
	}

	JSON SerialPort::toJSON(void)
	{
		return
		{
			{ "readingDelay",         readingDelay },
			{ "WriteTotalMultiplier", timeouts.WriteTotalTimeoutMultiplier },
			{ "ReadTotalMultiplier",  timeouts.ReadTotalTimeoutMultiplier},
			{ "ReadInterval",         timeouts.ReadIntervalTimeout},
			{ "ReadTotalConstant",    timeouts.ReadTotalTimeoutConstant},
			{ "WriteTotalConstant",   timeouts.WriteTotalTimeoutConstant}
		};
	}

	void SerialPort::fromJSON(const JSON& j)
	{
		DWORD WriteTotalTimeoutMultiplier, ReadTotalTimeoutMultiplier, ReadIntervalTimeout,
			ReadTotalTimeoutConstant, WriteTotalTimeoutConstant;

		j.at("readingDelay").get_to(readingDelay);
		j.at("WriteTotalMultiplier").get_to(WriteTotalTimeoutMultiplier);
		j.at("ReadTotalMultiplier").get_to(ReadTotalTimeoutMultiplier);
		j.at("ReadInterval").get_to(ReadIntervalTimeout);
		j.at("ReadTotalConstant").get_to(ReadTotalTimeoutConstant);
		j.at("WriteTotalConstant").get_to(WriteTotalTimeoutConstant);

		timeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;
		timeouts.ReadTotalTimeoutMultiplier  = ReadTotalTimeoutMultiplier;
		timeouts.ReadIntervalTimeout         = ReadIntervalTimeout;
		timeouts.ReadTotalTimeoutConstant    = ReadTotalTimeoutConstant;
		timeouts.WriteTotalTimeoutConstant   = WriteTotalTimeoutConstant;
	}
}
#endif

#ifdef __linux__

namespace RTPlot
{
	SerialPort::SerialPort(const std::string& _portName, uint32_t _baudRate, uint32_t _byteSize, sp_parity _parity, bool _verboseData) : 
		byteSize(_byteSize),
		parity(_parity), 
		connected(false),
		verboseData(_verboseData),
		baudRate(_baudRate), 
		portName(_portName),
		friendlyName("")
	{
		if (this->Connect()) this->CalcFrndlyName();
		else std::cerr << "[SerialPort]: Error constructing SerialPort object because couldn't connect with the serial port." << std::endl;
	}

	SerialPort::~SerialPort(void) 
	{
		this->Disconnect();
	}

	bool SerialPort::IsConnected(void) const
	{
        if (!port)
        {
            if (verboseData)
                std::cerr << "[SerialPort]: Invalid handle." << std::endl;
            return false;
        }

        sp_signal signals;
        sp_return result = sp_get_signals(port, &signals);
        if (result != SP_OK)
        {
            if (verboseData)
            {
                char* errorMsg = sp_last_error_message();
                std::cerr << "[SerialPort]: Error getting modem status. Error: " 
                          << (errorMsg ? errorMsg : "Unknown") << std::endl;
                sp_free_error_message(errorMsg);
            }
            return false;
        }
        
        return connected;
    }

    bool SerialPort::Connect(void)
    {
        // Get port object
        sp_return result = sp_get_port_by_name(portName.c_str(), &port);
        if (result != SP_OK || !port)
        {
            if (verboseData)
                std::cerr << "[SerialPort]: Couldn't connect. Invalid port name." << std::endl;
            return false;
        }

        // Try to open the port
        result = sp_open(port, SP_MODE_READ_WRITE);
        if (result != SP_OK)
        {
            if (verboseData)
            {
                char* errorMsg = sp_last_error_message();
                std::cerr << "[SerialPort]: Failed to open port " << portName << ". Error: " << (errorMsg ? errorMsg : "Unknown") << std::endl;
                sp_free_error_message(errorMsg);
            }
            sp_free_port(port);
            return false;
        }

        // Set port parameters
        if (sp_set_baudrate(port, baudRate) != SP_OK ||
            sp_set_bits(port, byteSize)     != SP_OK ||
            sp_set_parity(port, parity)     != SP_OK ||
            sp_set_stopbits(port, 1)        != SP_OK)
        {
            if (verboseData)
                std::cerr << "[SerialPort]: Failed to set COM port parameters." << std::endl;
            sp_close(port);
            sp_free_port(port);
            return false;
        }

        // Clear buffers
        sp_flush(port, SP_BUF_BOTH);

        if (verboseData)
            std::cout << "[SerialPort]: Connected to device at port " << portName << "." << std::endl;

        connected = true;
        return true;
    }

	bool SerialPort::Disconnect(void)
	{
		if (port)
        {
            sp_close(port);
            sp_free_port(port);
			return true;
        }
		else std::cerr << "[SerialPort]: The device trying to disconnect is not connected." << std::endl;
		return false;
	}

	void SerialPort::ClearBuffer(sp_buffer whichBuffer)
	{
		if (!port) return;
	
		enum sp_return result = sp_flush(port, whichBuffer);
		if (result != SP_OK)
		{
			std::cerr << "[SerialPort]: Error flushing buffer: " << sp_last_error_message() << std::endl;
		}
	}

	void SerialPort::CalcFrndlyName()
    {
        std::string sysPath = "/dev/serial/by-id/"; // Symlinks for device identifiers

        if (!std::filesystem::exists(sysPath))
        {
            if (verboseData) std::cerr << "[SerialPort]: Serial device path not found." << std::endl;
            return;
        }

        for (const auto &entry : std::filesystem::directory_iterator(sysPath))
        {
            std::string devicePath = entry.path();
            std::string deviceName = entry.path().filename();

            if (std::filesystem::canonical(devicePath) == std::filesystem::canonical(portName))
            {
                friendlyName = deviceName;
                return;
            }
        }

        if (verboseData)
            std::cerr << "[SerialPort]: Friendly name not found for port " << portName << std::endl;
    }

	int8_t SerialPort::Read(void* buf, size_t size)
    {
        if (!port) 
        {
            std::cerr << "[SerialPort]: Invalid handle value." << std::endl;
            return -1;
        }

        sp_return status;
        int bytesRead = 0;
        int bytesToRead = 0;

        usleep(readingDelay); 

        int availableBytes = sp_input_waiting(port);
        if (availableBytes < 0)
        {
            std::cerr << "[SerialPort]: Error checking input buffer." << std::endl;
            return RTPLOT_ERROR;
        }

        bytesToRead = (availableBytes >= size) ? size : availableBytes;
        if (bytesToRead < size)
            return RTPLOT_READING;

		static uint8_t readingCount = 0;
        if (readingCount >= 5)
        {
            sp_flush(port, SP_BUF_INPUT);
            readingCount = 0;
        }

        status = sp_blocking_read(port, buf, bytesToRead, 100);

		if (status == 0) 
			return RTPLOT_READING;

        if (status > 0)
        {
            readingCount = 0;
            return (status == bytesToRead) ? RTPLOT_FINISHED : RTPLOT_READING;
        }

		readingCount++;
		if (verboseData)
			std::cerr << "[SerialPort]: Error reading from COM port. Error: " << sp_last_error_message() << std::endl;
		return RTPLOT_ERROR;
    }

	int8_t SerialPort::Write(void* buf, size_t size)
	{
		static auto start_time = std::chrono::steady_clock::now();
		static bool delaying = true;
	
		if (!buf || size == 0)
			return RTPLOT_ERROR; // Invalid buffer or size
	
		if (!port) 
			return RTPLOT_ERROR; // Serial port not initialized
	
		// Check if still delaying
		if (delaying) 
		{
			auto now = std::chrono::steady_clock::now();
			int elapsed_time = std::chrono::duration_cast<millis_t>(now - start_time).count();
	
			if (elapsed_time < readingDelay)
				return RTPLOT_READING; // Still delaying
			else
				delaying = false; // Delay finished, proceed with writing
		}
	
		int bytes_written = sp_blocking_write(port, buf, size, 1000);
	
		if (bytes_written < 0)
			return RTPLOT_ERROR;
	
		return RTPLOT_OK;
	}

	std::vector<std::string> SerialPort::ScanAvailablePorts(void)
	{
		std::vector<std::string> ports;
		struct sp_port**         port_list;

		if (sp_list_ports(&port_list) == SP_OK)
		{
			for (size_t i = 0; port_list[i] != nullptr; i++)
			{
				const char* port_name = sp_get_port_name(port_list[i]);
				if (port_name) ports.push_back(port_name);
			}
			sp_free_port_list(port_list);
		}

		return ports;
	}

	JSON SerialPort::toJSON(void)
	{
		return
		{
			{ "readingDelay", readingDelay },
		};
	}

	void SerialPort::fromJSON(const JSON& j)
	{
		j.at("readingDelay").get_to(readingDelay);
	}
}

#endif