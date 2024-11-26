#include <serial/SerialDevice.h>

bool RTPlot::SerialDevice::Recieve(uint32_t delay)
{
    if (!port) { if (verboseData) std::cerr << "[SerialDevice]: Error dereferencing port: it is nullptr." << std::endl; return false; }
    if (!port->IsConnected()) { if (verboseData) std::cout << "[SerialDevice]: Device not connected." << std::endl; return false; }

    int8_t readCode = port->Read(reading, msgSize);
    if (readCode == RTPLOT_ERROR) { if (verboseData) std::cout << "[SerialDevice]: Could not read message from device." << std::endl; return false; }
    else if (readCode == RTPLOT_FINISHED)
    {
        if (reading == nullptr) { if (verboseData) std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

        char tempMsg[RTPLOT_MSG_SIZE];
        for (uint8_t i = 0; i < sizeof(tempMsg); i++)
        {
            tempMsg[i] = *((char*)reading + i);
            if (tempMsg[i] == '\n')
            {
                for (uint8_t j = i + 1; j < sizeof(tempMsg); j++)
                    tempMsg[j] = '\0';

                break;
            }
        }

        if ((tempMsg[1] == '+' || tempMsg[1] == '-')/* && tempMsg[sizeof(tempMsg) - 1] == '\n'*/)
        {
            char filteredMsg[15];
            for (uint8_t i = 0; i < sizeof(filteredMsg); i++)
            {
                filteredMsg[i] = tempMsg[i + 1];
            }
            dReading = atof(filteredMsg);

            if (verboseData) std::cout << "[" << port->GetName() << "]: Converted read data : " << dReading << std::endl;
            Sleep(RTPLOT_READING_DELAY);
        }
    }

    return true;
}

bool RTPlot::SerialDevice::Send(const std::string& msg)
{
    LPVOID message = (LPVOID)msg.c_str();
    if (port->Write(message, sizeof(message)) == RTPLOT_FINISHED)
        return true;
    else return false;
}

bool RTPlot::SerialDevice::ChangePort(const char* portName)
{
    if (!port) return false;

    if (reading) delete[] reading;
    port->Disconnect();

    reading = new void*;
    port->SetName(portName);

    if (port->Connect())
        return true;
    else return false;
}
