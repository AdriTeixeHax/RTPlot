#ifndef _INVERTERCONTROL__H_
#define _INVERTERCONTROL__H_

#include <Logger.h>
#include <SerialDevice.h>

namespace RTPlot
{
    enum class InverterState  { none, waiting, sixStep, spwm, svpwm };

    class InverterControl
    {
        ImGui::Log    log;
        std::string&  logMsg;
        char*         commandStr;
        bool*         sendCommand;

        int           spwmSelect = 0;

        InverterState state = InverterState::none;
        int32_t       pwmValue = 0;

    public:
        InverterControl(std::string& logMsgRef, char* commandPtr, bool* sendCommandPtr);
        InverterControl(const InverterControl& i);

        void ProcessStateChange(void);
        void ProcessSPWMStateChange(void);
        void ResetButton(void);
        void StopButton(void);
        void StartButton(void);
        void SPWMButton(void);
        void SixStepButton(void);

        void SendPWMCommand(void);

        void SixStepOperation(void);
        void SPWMOperation(void);

        void Draw(void);
    };
}

#endif