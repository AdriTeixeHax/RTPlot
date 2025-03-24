#include <serial/SerialDevice.h>

#ifdef _WIN32
RTPlot::SerialDevice::SerialDevice(const char* portName, size_t size, uint32_t baudRate) : 
    port(new RTPlot::SerialPort(portName, baudRate, 8U, PARITY_EVEN)), 
    readingRaw("\0")
{ 
    port->ClearBuffer(); 
    for (uint8_t i = 0; i < RTPLOT_MAX_DATA_NUM; i++) readingVals.push_back(0);
}
#endif

#ifdef __linux__
RTPlot::SerialDevice::SerialDevice(const char* portName, size_t size, uint32_t baudRate) : 
    port(new RTPlot::SerialPort(portName)), 
    readingRaw("\0")
{ 
    port->ClearBuffer(); 
    for (uint8_t i = 0; i < RTPLOT_MAX_DATA_NUM; i++) readingVals.push_back(0);
}
#endif

RTPlot::SerialDevice::~SerialDevice(void) 
{ 
    delete port; 
}

bool RTPlot::SerialDevice::Recieve(void)
{
    // Error checking
    if (!port) { if (verboseData) std::cerr << "[SerialDevice]: Error dereferencing port: it is nullptr." << std::endl; return false; }
    if (!port->IsConnected()) { if (verboseData) std::cout << "[SerialDevice]: Device not connected." << std::endl; return false; }

    // Reset reading variable
    strcpy_s(readingRaw, sizeof(""), "");

    // Reading from port and error checking
    int8_t readCode = port->Read(readingRaw, sizeof(readingRaw));

    if (strcmp(readingRaw, "") == 0) readCode = RTPLOT_READING;

    switch (readCode)
    {
    case RTPLOT_ERROR:
        if (verboseData) std::cout << "[SerialDevice]: Could not read message from device." << std::endl; 
        return false;
        break;

    case RTPLOT_READING:
        return true;

    case RTPLOT_FINISHED:
        // Error checking
        if (readingRaw == nullptr) { if (verboseData) std::cout << "[SerialDevice]: Reading returned nullptr." << std::endl; return false; }

        ProcessData();
        PrintData();

        return true;
        break;

    default:
        return false;
    }
}

#ifdef _WIN32

bool RTPlot::SerialDevice::Send(const char* msg, uint32_t len)
{
    LPVOID message = (LPVOID)msg;
    if (port->Write(message, len) == RTPLOT_FINISHED)
    {
        return true;
    }
    else return false;
}

#endif

#ifdef __linux__

bool RTPlot::SerialDevice::Send(const char* msg, uint32_t len)
{
    void* message = (void*)msg;
    if (port->Write(message, len) == RTPLOT_FINISHED)
    {
        return true;
    }
    else return false;
}

#endif

int8_t RTPlot::SerialDevice::ProcessData(void)
{
    char tempMsg[RTPLOT_MSG_SIZE] = { 0 };
    bool startFlag = false;
    bool endFlag = false;
    size_t x = 0;

    for (size_t i = 0; i < sizeof(tempMsg); i++)
    {

        // If the byte read is a 'b' (begin), then start copying the message
        if (readingRaw[i] == 'b') startFlag = true;
        if (startFlag && readingRaw[i] != 'b' && readingRaw[i] != 'e')
        {
            tempMsg[x] = readingRaw[i];
            x++;
        }

        if (startFlag && readingRaw[i] == 'e') endFlag = true;
        if (endFlag && i < sizeof(tempMsg))
        {
            // Fill the rest with 0s.
            for (size_t j = x; j < sizeof(tempMsg); j++)
                tempMsg[j] = '\0';

            // Exit the main "for" loop
            break;
        }
    }

    std::cout << "[RawMsg]: " << readingRaw << std::endl;
    std::cout << "[TempMsg]: " << tempMsg << std::endl;

    // Process the data
    char finalMsg[RTPLOT_MAX_DATA_NUM][RTPLOT_DATA_SIZE] = { 0 };
    size_t msgSel = 0;
    size_t k = 0;
    for (size_t i = 0; i < sizeof(tempMsg); i++)
    {
        if (tempMsg[i] == ',' || k >= RTPLOT_DATA_SIZE)
        {
            k = 0;
            msgSel++;
        }
        else if (msgSel >= RTPLOT_MAX_DATA_NUM)
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

    // Cast the data into a double, unless t = 0 (invalid)
    if (atof(finalMsg[0]) < RTPLOT_TOLERANCE_DELTA) return RTPLOT_ERROR;
    for (uint8_t i = 0; i < RTPLOT_MAX_DATA_NUM; i++)
        readingVals.at(i) = atof(finalMsg[i]);

    return RTPLOT_OK;
}

void RTPlot::SerialDevice::PrintData(void)
{
    if (verboseData)
    {
        // std::cout << "[" << port->GetName() << "]: Converted read data: ";

        // for (uint8_t i = 0; i < readingVals.size(); i++)
        // {
        //     std::cout << readingVals.at(i);
        //     if (i < RTPLOT_MAX_DATA_NUM - 1)
        //         std::cout << ", ";
        // }
        // std::cout << std::endl;
    }
}

JSON RTPlot::SerialDevice::toJSON(void)
{
    return JSON
    {
        {"verboseData", verboseData},
        {"port", port->toJSON()},
    };
}

void RTPlot::SerialDevice::fromJSON(const JSON& j)
{
    j.at("verboseData").get_to(verboseData);
    port->fromJSON(j.at("port"));
}
