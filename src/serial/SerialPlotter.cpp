#include <serial/SerialPlotter.h>
#include <RTPlotFunctions.h>

RTPlot::SerialPlotter::SerialPlotter(const char* _port, std::string* _logMsg) :
	realTimePlotter(new RealTimePlot),
	serialDevice(new SerialDevice(_port)),
    logMsgPtr(_logMsg)
{
	serialCommThread = std::thread(&SerialPlotter::SerialFunc, this);
	strcpy_s(commandToSend, "");
}

RTPlot::SerialPlotter::SerialPlotter(const SerialPlotter& s)
{
    this->killFlag           = s.killFlag;
    this->addVariable        = s.addVariable;
    this->removeVariable     = s.removeVariable;
    this->varToRemove        = s.varToRemove;
    this->exitCommThreadFlag = s.exitCommThreadFlag;
    this->sendCommand        = s.sendCommand;

    this->logMsgPtr          = s.logMsgPtr;

    this->realTimePlotter    = new RealTimePlot(*s.realTimePlotter);
    this->serialDevice       = new SerialDevice(*s.serialDevice);

    strcpy_s(this->commandToSend, s.commandToSend);
}

RTPlot::SerialPlotter::~SerialPlotter(void)
{
    exitCommThreadFlag = true;
    serialCommThread.join();
    delete serialDevice;
    delete realTimePlotter;
}

void RTPlot::SerialPlotter::Plot(const std::string& portName)
{
    mutex.lock();
        realTimePlotter->Plot(portName, serialDevice->GetFriendlyPortName(), &killFlag, commandToSend, &sendCommand, &addVariable, &varToRemove, &removeVariable);
        for (size_t i = 0; i < realTimePlotter->GetPlotters()->size(); i++)
        {
            // Delete element if its kill flag is true
            if (realTimePlotter->GetPlotters()->at(i)->GetKillPlot() == true)
            {
                delete realTimePlotter->GetPlotters()->at(i);
                realTimePlotter->GetPlotters()->erase(realTimePlotter->GetPlotters()->begin() + i);
            }
        }

        this->SerialOptionsWindow(realTimePlotter->GetSerialOptionsFlagPtr(), logMsgPtr);
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
        static int wtm = 10, rtm = 10, ri = 50, rtc = 1000, wtc = 1000; // Serial parameters
        static int readingDelay = 5;

        ImGui::Begin(std::string(StripPortNamePrefix(GetPortName()) + " - Serial options").c_str(), serialOptionsFlag);
            if (ImGui::Button("Apply"))
            {
                serialDevice->GetPort()->SetTimeouts(wtm, rtm, ri, rtc, wtc);
                serialDevice->GetPort()->SetReadingDelay(abs(readingDelay)); // abs just in case some negative number ends up there.
                *logMsg = "Applied serial parameters for port " + StripPortNamePrefix(GetPortName()) + ".\n";
            }

            COMMTIMEOUTS timeouts = serialDevice->GetPort()->GetTimeouts();
            // Extra label (##x) just in case the numbers coincide
            std::string rdlText  = "(Curr. value: " + std::to_string(serialDevice->GetPort()->GetReadingDelay()) + ")##1";
            std::string wtmText = "(Curr. value: " + std::to_string(timeouts.WriteTotalTimeoutMultiplier)       + ")##2";
            std::string wtcText = "(Curr. value: " + std::to_string(timeouts.WriteTotalTimeoutConstant)         + ")##3";
            std::string rtmText = "(Curr. value: " + std::to_string(timeouts.ReadTotalTimeoutMultiplier)        + ")##4";
            std::string rtcText = "(Curr. value: " + std::to_string(timeouts.ReadTotalTimeoutConstant)          + ")##5";
            std::string rdiText = "(Curr. value: " + std::to_string(timeouts.ReadIntervalTimeout)               + ")##6";

            ImGui::SeparatorText("RTPlot port options");
            ImGui::Text("Reading Delay:");
            ImGui::InputInt(rdlText.c_str(), &readingDelay);

            ImGui::SeparatorText("Windows port options");
            ImGui::Text("Write Total Multiplier:");
            ImGui::InputInt(wtmText.c_str(), &wtm);
            ImGui::Text("Write Total Constant:");
            ImGui::InputInt(wtcText.c_str(), &wtc);
            ImGui::Text("Read Total Multiplier:");
            ImGui::InputInt(rtmText.c_str(), &rtm);
            ImGui::Text("Read Total Constant:");
            ImGui::InputInt(rtcText.c_str(), &rtc);
            ImGui::Text("Read Interval:");
            ImGui::InputInt(rdiText.c_str(), &ri);
        ImGui::End();
    }
}

void RTPlot::SerialPlotter::SaveConfig(const std::string& filepath)
{
    JSON          json = this->toJSON();
    std::ofstream file(filepath);

    if (file) file << json.dump(4); // Pretty print JSON with 4-space indentation
    else std::cerr << "[SerialPlotter]: Failed to open file for writing!" << std::endl;
}

void RTPlot::SerialPlotter::LoadConfig(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (file)
    {
        JSON j;
        file >> j;
        this->fromJSON(j);
    }
    else std::cerr << "[SerialPlotter]: Failed to open file for reading!" << std::endl;
}

void RTPlot::SerialPlotter::SerialFunc(void)
{
    while (!exitCommThreadFlag)
    {
        if (!serialDevice->Recieve()) killFlag = false;

        mutex.lock();
            realTimePlotter->SetDataToPlot(serialDevice->GetReadingVals());
		mutex.unlock();

        if (sendCommand)
        {
            serialDevice->Send(commandToSend, sizeof(commandToSend));
            sendCommand = false;
        }
    }
}

JSON RTPlot::SerialPlotter::toJSON(void)
{
    return JSON
    {
        { "serialDevice", serialDevice->toJSON()},
        { "realTimePlotter", realTimePlotter->toJSON() }
    };
}

void RTPlot::SerialPlotter::fromJSON(const JSON& j)
{
    serialDevice->fromJSON(j.at("serialDevice"));
    realTimePlotter->fromJSON(j.at("realTimePlotter"));
}
