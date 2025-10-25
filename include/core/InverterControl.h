#ifndef _INVERTERCONTROL__H_
#define _INVERTERCONTROL__H_

#include <Logger.h>
#include <SerialDevice.h>
#include <FileManager.h>

#define SPWM_MAX_WIDTH 600

namespace RTPlot
{
    enum class InverterState  { none, waiting, sixStep, spwm, svpwm, pid };

    class InverterControl
    {
        ImGui::Log    log;
        std::string&  logMsg;
        char*         commandStr;
        bool*         sendCommand;
        bool          logToFileFlag = false;

        int           spwmSelect = 0;

        InverterState state = InverterState::none;
        int32_t       pwmValue = 0;

        FileManager   fileManager;

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
        void PIDButton(void);

        void SendPWMCommand(void);
        void IVFilteringTweak(void);
        void CheckAndSend(float* valueArray, const char* name);

        void PIDTweak(void);
        void SixStepOperation(void);
        void SPWMOperation(void);

        void Draw(void);
    };
}

#endif