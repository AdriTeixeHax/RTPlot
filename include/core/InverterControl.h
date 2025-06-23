#ifndef _INVERTERCONTROL__H_
#define _INVERTERCONTROL__H_

#include <Logger.h>
#include <SerialDevice.h>

namespace RTPlot
{
    class InverterControl
    {
        ImGui::Log   log;
        std::string& logMsg;
        char*        commandStr;
        bool*        sendCommand;

        bool         spwmFlag = false;
        int          spwmSelect = 0;

    public:
        InverterControl(std::string& logMsgRef, char* commandPtr, bool* sendCommandPtr);
        InverterControl(const InverterControl& i);
        void Draw(void);
    };
}

#endif