#include <serial/SerialDevice.h>

RTPlot::SerialDevice::SerialDevice(const char* portName, size_t size, uint32_t baudRate) : 
    port(new RTPlot::SerialPort(portName, baudRate, 8U, PARITY_EVEN)), 
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

    // Reset reading variable
    strcpy_s(reading, "");

    // Reading from port and error checking
    int8_t readCode = port->Read(reading, sizeof(reading));
    if (readCode == RTPLOT_ERROR) { if (verboseData) std::cout << "[SerialDevice]: Could not read message from device." << std::endl; return false; }
    
    // If the reading is correct, process the data
    if (readCode == RTPLOT_FINISHED)
    {
        if (reading == nullptr) { if (verboseData) std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

        ProcessData();

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

        return true;
    }

    return false;
}

bool RTPlot::SerialDevice::Send(const char* msg, uint32_t len)
{
    LPVOID message = (LPVOID)msg;
    if (port->Write(message, len) == RTPLOT_FINISHED)
    {
        return true;
    }
    else return false;
}

int8_t RTPlot::SerialDevice::ProcessData(void)
{
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

        if (startFlag && reading[i] == 'e') endFlag = true;
        if (endFlag && i < sizeof(tempMsg))
        {
            // Fill the rest with 0s.
            for (size_t j = x; j < sizeof(tempMsg); j++)
                tempMsg[j] = '\0';

            // Exit the main "for" loop
            break;
        }
    }

    // Process the data
    char finalMsg[RTPLOT_DATA_NUM][RTPLOT_DATA_SIZE] = { 0 }; // TODO: Needs to be according to current size
    size_t msgSel = 0;
    size_t k = 0;
    for (size_t i = 0; i < sizeof(tempMsg); i++)
    {
        if (tempMsg[i] == ',' || k >= RTPLOT_DATA_SIZE)
        {
            k = 0;
            msgSel++;
        }
        else if (msgSel >= RTPLOT_DATA_NUM)
        {
            msgSel = 0;
            return RTPLOT_ERROR;
        }
        else
        {
            finalMsg[msgSel][k] = tempMsg[i];
            if (k < RTPLOT_DATA_SIZE - 1) k++;
            else
                break;
        }
    }

    // Cast the data into a double.
    for (uint8_t i = 0; i < RTPLOT_DATA_NUM; i++)
        dReading.at(i) = atof(finalMsg[i]);

    return RTPLOT_OK;
}