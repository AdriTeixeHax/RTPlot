#include <SerialPort.h>
#include <RTPlotFunctions.h>

void SerialPort_UnitTest(void)
{
    struct sp_port *port;
    sp_get_port_by_name("/dev/ttyACM0", &port);
    sp_open(port, SP_MODE_READ_WRITE);
    sp_set_baudrate(port, 115200);

    char buf[256];
    int bytes_read = sp_blocking_read(port, buf, sizeof(buf), 1000);
    std::cout << "Read " << bytes_read << " bytes: " << buf << std::endl;

    for (uint32_t i = 0; i < 10; i++)
    {
        char msg[256];
        sp_blocking_read(port, msg, sizeof(msg), 1000);
        std::cout << "Read message: " << msg << std::endl;
        sp_flush(port, SP_BUF_BOTH);
    }

    sp_close(port);
    sp_free_port(port);

    std::vector<std::string> availablePorts = RTPlot::SerialPort::ScanAvailablePorts();

    std::cout << "Available Serial Ports:\n";
    for (const auto &port : availablePorts)
    {
        std::cout << port << std::endl;
    }

    RTPlot::SerialPort serialPort("/dev/ttyACM0");
    std::cout << "Is connected: " << serialPort.IsConnected() << std::endl;
    std::cout << "Friendly name: " << serialPort.GetFriendlyName() << std::endl;

    for (uint32_t i = 0; i < 10; i++)
    {
        char msg[256];
        serialPort.Read(msg, sizeof(msg));
        std::cout << "Read message: " << msg << std::endl;
    }

    // Write test
    const char* testData = "Hello, Serial!";
    size_t dataSize = strlen(testData);

    // Write data to serial port
    int8_t returnCode = serialPort.Write((void*)testData, dataSize);

    if (returnCode == RTPLOT_OK) std::cout << "Successfully wrote to the serial port." << std::endl; 
    else std::cerr << "Failed to write to serial port!" << std::endl;
}

int main(void)
{
    SerialPort_UnitTest();
    return 0;
}