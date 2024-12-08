#include <serial/SerialDevice.h>

RTPlot::SerialDevice::SerialDevice(const char* portName, size_t size, uint32_t baudRate) : 
    port(new RTPlot::SerialPort(portName, baudRate)), 
    reading("\0")
{ 
    port->ClearBuffer(); 
    for (uint8_t i = 0; i < RTPLOT_DATA_NUM; i++) dReading.push_back(0);
}

RTPlot::SerialDevice::~SerialDevice(void) 
{ 
    delete port; 
}

bool RTPlot::SerialDevice::Recieve(uint32_t delay)
{
    // Error checking
    if (!port) { if (verboseData) std::cerr << "[SerialDevice]: Error dereferencing port: it is nullptr." << std::endl; return false; }
    if (!port->IsConnected()) { if (verboseData) std::cout << "[SerialDevice]: Device not connected." << std::endl; return false; }

    // Reading from port and error checking
    int8_t readCode = port->Read(reading, RTPLOT_MSG_SIZE);
    //if (readCode == RTPLOT_ERROR) { if (verboseData) std::cout << "[SerialDevice]: Could not read message from device." << std::endl; return false; }
    
    // If the reading is correct, process the data
    //else if (readCode == RTPLOT_FINISHED)
    //{
    //    if (reading == nullptr) { if (verboseData) std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

    //    // Convert the void* data into char*. ';' marks the end of the message, and the rest is filled with '\0'.
    //    char tempMsg[RTPLOT_MSG_SIZE] = { 0 };
    //    for (uint8_t i = 0; i < sizeof(tempMsg); i++)
    //    {
    //        tempMsg[i] = *((char*)reading + i);
    //        if (tempMsg[i] == ';')
    //        {
    //            for (uint8_t j = i + 1; j < sizeof(tempMsg); j++)
    //                tempMsg[j] = '\0';

    //            break;
    //        }
    //    }

    //    //if ((tempMsg[1] == '+' || tempMsg[1] == '-')/* && tempMsg[sizeof(tempMsg) - 1] == '\n'*/)
    //    //{
    //    //    char filteredMsg[15];
    //    //    for (uint8_t i = 0; i < sizeof(filteredMsg); i++)
    //    //    {
    //    //        filteredMsg[i] = tempMsg[i + 1];
    //    //    }
    //    //    dReading = atof(filteredMsg);

    //    //    if (verboseData) std::cout << "[" << port->GetName() << "]: Converted read data : " << dReading << std::endl;
    //    //    Sleep(RTPLOT_READING_DELAY);
    //    //}

    //    // Process the data
    //    char finalMsg[RTPLOT_DATA_NUM][RTPLOT_DATA_SIZE] = { 0 };
    //    static uint8_t msgSel = 0;
    //    static uint8_t k = 0;
    //    //for (uint8_t i = 0; i < sizeof(tempMsg); i++)
    //    //{
    //    //    k += i;
    //    //    if (tempMsg[i] == ',') 
    //    //    {
    //    //        k = 0;
    //    //        msgSel++;
    //    //    }
    //    //    else if (tempMsg[i] == ';') 
    //    //    {
    //    //        msgSel = 0; 
    //    //        k = 0;
    //    //        break;
    //    //    }
    //    //    else finalMsg[msgSel][k % RTPLOT_DATA_SIZE] = tempMsg[i];
    //    //}

    //    //// Cast the data into a double if the message is correct
    //    //if (finalMsg[RTPLOT_DATA_NUM - 1][RTPLOT_DATA_SIZE - 1] == ';')
    //    //{
    //    //    for (uint8_t i = 0; i < dReading.size(); i++)
    //    //    {
    //    //        dReading.at(i) = atof(finalMsg[i]);
    //    //    }
    //    //}

    //    //if (verboseData)
    //    //{
    //    //    std::cout << "[" << port->GetName() << "]: Converted read data: ";

    //    //    for (uint8_t i = 0; i < sizeof(finalMsg); i++)
    //    //    {
    //    //        std::cout << finalMsg[i];
    //    //        if (i < RTPLOT_DATA_NUM - 1)
    //    //            std::cout << ", ";
    //    //    }
    //    //    std::cout << std::endl;
    //    //}

    //    Sleep(RTPLOT_READING_DELAY);
    //}

    return true;
}

bool RTPlot::SerialDevice::Send(const std::string& msg)
{
    LPVOID message = (LPVOID)msg.c_str();
    if (port->Write(message, sizeof(message)) == RTPLOT_FINISHED)
        return true;
    else return false;
}