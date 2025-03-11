#include <serial/SerialPlotter.h>
#include <RTPlotFunctions.h>

RTPlot::SerialPlotter::SerialPlotter(const char* _port, std::string* _logMsg) :
	plotter(new RealTimePlot),
	serialDevice(new SerialDevice(_port)),
    logMsgPtr(_logMsg)
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
        plotter->Plot(portName, serialDevice->GetFriendlyPortName(), &killFlag, commandToSend, &sendCommand, &addVariable, &varToRemove, &removeVariable);
        for (size_t i = 0; i < plotter->GetPlotters()->size(); i++)
        {
            // Delete element if its kill flag is true
            if (plotter->GetPlotters()->at(i)->GetKillPlot() == true)
            {
                delete plotter->GetPlotters()->at(i);
                plotter->GetPlotters()->erase(plotter->GetPlotters()->begin() + i);
            }
        }

        this->SerialOptionsWindow(plotter->GetSerialOptionsFlagPtr(), logMsgPtr);
    mutex.unlock();

    if (addVariable)
    {
        serialDevice->GetReadingPtr()->push_back(0.0f);
        addVariable = false;
    }
}

void RTPlot::SerialPlotter::SerialOptionsWindow(bool* serialOptionsFlag, std::string* logMsg)
{
    if (*serialOptionsFlag)
    {
        ImGui::Begin(std::string(StripPortNamePrefix(GetPortName()) + " - Serial options").c_str(), serialOptionsFlag);
        static int wtm = 10, rtm = 10, ri = 50, rtc = 1000, wtc = 1000; // Serial parameters
        static int readingDelay = 5;

        if (ImGui::Button("Apply"))
        {
            serialDevice->GetPort()->SetTimeouts(wtm, rtm, ri, rtc, wtc);
            serialDevice->GetPort()->SetReadingDelay(abs(readingDelay)); // abs just in case some negative number ends up there.
            *logMsg = "Applied serial parameters for port " + StripPortNamePrefix(GetPortName()) + ".\n";
        }

        ImGui::SeparatorText("RTPlot port options");
        ImGui::InputInt("Reading Delay", &readingDelay);

        ImGui::SeparatorText("Windows port options");
        ImGui::InputInt("Write Total Multiplier", &wtm);
        ImGui::InputInt("Write Total Constant",   &wtc);
        ImGui::InputInt("Read Total Multiplier",  &rtm);
        ImGui::InputInt("Read Total Constant",    &rtc);
        ImGui::InputInt("Read Interval",          &ri);

        ImGui::End();
    }
}

void RTPlot::SerialPlotter::SerialFunc(void)
{
    while (!exitCommThreadFlag)
    {
        if (!serialDevice->Recieve()) killFlag = false;

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