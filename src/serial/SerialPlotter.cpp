#include <serial/SerialPlotter.h>
#include <RTPlotVars.h>

RTPlot::SerialPlotter::SerialPlotter(const char* port, Graphics* graphicsPtr) :
	plotter(new RealTimePlot(graphicsPtr)),
	serialDevice(new SerialDevice(port))
{
    this->graphicsPtr = graphicsPtr;  
	serialCommThread = std::thread(&SerialPlotter::SerialFunc, this);

	strcpy_s(commandToSend, "");
}

RTPlot::SerialPlotter::~SerialPlotter(void)
{
    exitCommThreadFlag = true;
    serialCommThread.join();
    delete serialDevice;
    delete plotter;
}

std::string RTPlot::SerialPlotter::GetPortNameGUI(void) const 
{
    std::string name = this->GetPortName();
    size_t pos; // Iterator
    while ((pos = name.find(LONG_COM_PORT_PREFIX)) != std::string::npos)
        name.erase(pos, strlen(LONG_COM_PORT_PREFIX));
    return name;
}

void RTPlot::SerialPlotter::Plot(const std::string& portName)
{
    plotter->Plot(portName, &killFlag, commandToSend, &sendCommand, &addVariable, &varToRemove, &removeVariable);
    if (addVariable)
    {
        serialDevice->GetReadingPtr()->push_back(0.0f);
        addVariable = false;
    }
}

void RTPlot::SerialPlotter::SerialFunc(void)
{
    while (!exitCommThreadFlag)
    {
        serialDevice->Recieve();
        plotter->SetDataToPlot(serialDevice->GetReadingVals());

        if (sendCommand)
        {
            serialDevice->Send(commandToSend, sizeof(commandToSend));
            sendCommand = false;
        }
    }
}