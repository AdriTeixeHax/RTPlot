#include <serial/SerialPlotter.h>
#include <core/RTPlotVars.h>

RTPlot::SerialPlotter::SerialPlotter(const char* port) :
	plotter(new RealTimePlot),
	serialDevice(new SerialDevice(port))
{
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

void RTPlot::SerialPlotter::Plot(const std::string& portName)
{
    mutex.lock();
    plotter->Plot(portName, &killFlag, commandToSend, &sendCommand, &addVariable, &varToRemove, &removeVariable);
	mutex.unlock();

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

        mutex.lock();
        plotter->SetDataToPlot(serialDevice->GetReadingVals());
		mutex.unlock();

        if (sendCommand)
        {
            serialDevice->Send(commandToSend, sizeof(commandToSend));
            sendCommand = false;
        }
    }
}