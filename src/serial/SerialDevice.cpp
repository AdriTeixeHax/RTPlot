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
    if (readCode == RTPLOT_ERROR) { if (verboseData) std::cout << "[SerialDevice]: Could not read message from device." << std::endl; return false; }
    
    // If the reading is correct, process the data
    else if (readCode == RTPLOT_FINISHED)
    {
        if (reading == nullptr) { if (verboseData) std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

        char tempMsg[RTPLOT_MSG_SIZE] = { 0 };
        bool startFlag = false;
        bool endFlag = false;
        size_t x = 0;
        for (size_t i = 0; i < sizeof(tempMsg); i++)
        {

            // If the byte read is a 'b' (begin), then start copying the message
            if (reading[i] == 'b') startFlag = true;
            if (startFlag && reading[i] != 'b' && reading[i] != 'e')
            {
                tempMsg[x] = reading[i];
                x++;
            }

            if (reading[i] == 'e') endFlag = true;
            if (endFlag && i < sizeof(tempMsg))
            {
                // Fill the rest with 0s.
                for (size_t j = x - 1; j < sizeof(tempMsg); j++)
                    tempMsg[j] = '\0';

                // Exit the "for" loop
                break;
            }
        }

        // Process the data
        char finalMsg[RTPLOT_DATA_NUM][RTPLOT_DATA_SIZE] = { 0 };
        size_t msgSel = 0;
        size_t k = 0;
        for (size_t i = 0; i < sizeof(tempMsg); i++)
        {
            if (tempMsg[i] == ',') 
            {
                k = 0;
                msgSel++;
            }
            else 
            {
                finalMsg[msgSel][k] = tempMsg[i];
                if (k < RTPLOT_DATA_SIZE - 1) k++;
                else break;
            }
        }

        // Cast the data into a double and filter the random 0s.
        for (uint8_t i = 0; i < RTPLOT_DATA_NUM; i++)
        {
            if (abs(atof(finalMsg[i])) >= RTPLOT_TOLERANCE)
                dReading.at(i) = atof(finalMsg[i]);
        }

        if (verboseData)
        {
            std::cout << "[" << port->GetName() << "]: Converted read data: ";

            for (uint8_t i = 0; i < dReading.size(); i++)
            {
                std::cout << dReading.at(i);
                if (i < RTPLOT_DATA_NUM - 1)
                    std::cout << ", ";
            }
            std::cout << std::endl;
        }

        Sleep(RTPLOT_READING_DELAY);
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