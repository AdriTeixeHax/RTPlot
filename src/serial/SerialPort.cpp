#include "SerialPort.h"

namespace RTPlot
{
	SerialPort::SerialPort(const char* port) : hCOM((void*)0), status({ 0 }), errors(0), connected(false)
	{
		hCOM = CreateFileA(static_cast<LPCSTR>(port), GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		DWORD error = GetLastError();
		if (error == ERROR_FILE_NOT_FOUND) std::cerr << "[SerialPort]: Device not connected at port " << port << "." << std::endl;
		else if (error == ERROR_ACCESS_DENIED)  std::cerr << "[SerialPort]: Device being used by another process at port " << port << "." << std::endl;
		else if (error == 0) // OK
		{
			DCB parameters = { 0 }; // DCB == Device Control Block

			if (!GetCommState(hCOM, &parameters)) std::cout << "[SerialPort]: Failed to get serial parameters." << std::endl;
			else
			{
				parameters.BaudRate = CBR_115200;
				parameters.ByteSize = RTPLOT_BYTE_SIZE;
				parameters.StopBits = ONESTOPBIT;
				parameters.Parity = NOPARITY;
				parameters.fDtrControl = DTR_CONTROL_ENABLE;

				if (!SetCommState(hCOM, &parameters)) std::cerr << "[SerialPort]: Could not set COM port parameters." << std::endl;
				else
				{
					connected = true;
					PurgeComm(hCOM, PURGE_RXCLEAR | PURGE_TXCLEAR);
					std::cout << "[SerialPort]: Connected to device at COM port " << port << "." << std::endl;
				}
			}
		}
	}

	SerialPort::~SerialPort(void)
	{
		if (connected == true)
		{
			connected = false;
			CloseHandle(hCOM);
		}
	}

	int8_t SerialPort::read(char* buf, size_t size)
	{
		clock_t begin = clock();

		DWORD bytesRead;
		uint32_t bytesToRead = 0;

		ClearCommError(hCOM, &errors, &status);
		
		if (status.cbInQue > 0)
		{
			if (status.cbInQue > size) bytesToRead = size;
			else bytesToRead = status.cbInQue;
		}
		
		
		if (ReadFile(hCOM, buf, bytesToRead, &bytesRead, NULL))
		{
			if (bytesRead == bytesToRead) return RTPLOT_FINISHED;
			else if (bytesRead < bytesToRead) return RTPLOT_READING;
			else if (bytesRead > bytesToRead) return RTPLOT_ERROR;
		}
	}
}
