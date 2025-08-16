#include <InverterControl.h>

namespace RTPlot
{
    InverterControl::InverterControl(std::string& logMsgRef, char* commandPtr, bool* sendCommandPtr) :
        log("Inverter Log"),
        logMsg(logMsgRef),
        commandStr(commandPtr),
        sendCommand(sendCommandPtr)
    {

    }

    InverterControl::InverterControl(const InverterControl& i) :
        log(i.log.name),
        logMsg(i.logMsg),
        commandStr(i.commandStr),
        sendCommand(i.sendCommand)
    {

    }

    void InverterControl::ProcessStateChange(void)
    {
        if      (logMsg == "[RTSpeed Conf]: none")      { state = InverterState::none;       logMsg = ""; }
        else if (logMsg == "[RTSpeed Conf]: waiting")   { state = InverterState::waiting;    logMsg = ""; }
        else if (logMsg == "[RTSpeed Conf]: sixStep")   { state = InverterState::sixStep;    logMsg = ""; }
        else if (logMsg == "[RTSpeed Conf]: spwm")      { state = InverterState::spwm;       logMsg = ""; }
        else if (logMsg == "[RTSpeed Conf]: svpwm")     { state = InverterState::svpwm;      logMsg = ""; }

        if (logToFileFlag == true)
        {
            static std::string prevMsg = "";

            if (prevMsg != logMsg)
                fileManager.Append("logs/configurationLog.txt", logMsg);

            prevMsg = logMsg;
        }
    }

    void InverterControl::ProcessSPWMStateChange(void)
    {
        if      (logMsg == "[RTSpeed Conf]: noTHIPWM")  { spwmSelect = 0; logMsg = ""; }
        else if (logMsg == "[RTSpeed Conf]: triTHIPWM") { spwmSelect = 1; logMsg = ""; }
        else if (logMsg == "[RTSpeed Conf]: sinTHIPWM") { spwmSelect = 2; logMsg = ""; }
    }

    void InverterControl::ResetButton(void)
    {
        // Red button
        ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.5f));
        if (ImGui::Button("Reset inverter"))
        {
            strcpy(commandStr, "RTSPEED_RESET");
            *sendCommand = true;
            logToFileFlag = false;
        }
        ImGui::PopStyleColor(3);
    }

    void InverterControl::StopButton(void)
    {
        // Red Button
        ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.5f));
        if (ImGui::Button("Stop operation"))
        {
            strcpy(commandStr, "RTSPEED_STOP");
            *sendCommand = true;
        }
        ImGui::PopStyleColor(3);
    }

    void InverterControl::StartButton(void)
    {
        // Green Button
        ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
        if (ImGui::Button("Start inverter"))
        {
            strcpy(commandStr, "RTSPEED_START");
            *sendCommand = true;
            logToFileFlag = true;
        }
        ImGui::PopStyleColor(3); 
    }

    void InverterControl::SPWMButton(void)
    {
        // Green Button
        ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
        if (ImGui::Button("Start SPWM"))
        {
            strcpy(commandStr, "RTSPEED_BEGIN_SPWM");
            *sendCommand = true;
        }
        ImGui::PopStyleColor(3);
    }

    void InverterControl::SixStepButton(void)
    {
        // Green Button
        ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
        if (ImGui::Button("Start 6 Step Algorithm"))
        {
            strcpy(commandStr, "RTSPEED_BEGIN_6STEP");
            *sendCommand = true;
        }
        ImGui::PopStyleColor(3);
    }

    void InverterControl::SendPWMCommand(void)
    {
        char tempMsg[256];
        snprintf(tempMsg, sizeof(tempMsg), "bspeedPWM:%d;", pwmValue);
        strcpy(commandStr, tempMsg);
        *sendCommand = true;
    }

    void InverterControl::SixStepOperation(void)
    {
        ImGui::SetNextItemWidth(150);
        ImGui::InputInt("PWM value", &pwmValue, 1, 10);
        ImGui::SameLine();
        ImGui::SetNextItemWidth(300);
        ImGui::SliderInt("##", &pwmValue, 0, 255);

        static int32_t prevValue = 0;

        if (prevValue != pwmValue)
            SendPWMCommand();

        prevValue = pwmValue;
    }

    void InverterControl::SPWMOperation(void)
    {
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ProcessSPWMStateChange();
        ImGui::BeginChild("SPWM configuration", ImVec2(avail.x, avail.y / 2), ImGuiChildFlags_Border);
            ImGui::Text("SPWM modifier wave: ");
            ImGui::SameLine();
            ImGui::RadioButton("None",                &spwmSelect, 0); ImGui::SameLine();
            ImGui::RadioButton("Triangular Modifier", &spwmSelect, 1); ImGui::SameLine();
            ImGui::RadioButton("Sinusoidal Modifier", &spwmSelect, 2);

            static uint8_t previousButton = 0;
            if (previousButton != spwmSelect)
            {
                switch (spwmSelect)
                {
                    case 0: strcpy(commandStr, "RTSPEED_NONE_SPWM"); break;
                    case 1: strcpy(commandStr, "RTSPEED_TRI_SPWM");  break;
                    case 2: strcpy(commandStr, "RTSPEED_SIN_SPWM");  break;
                    default: break;
                }
                *sendCommand = true;

                previousButton = spwmSelect;
            }
            
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            ImGui::InputInt("PWM value", &pwmValue, 1, 10);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(300);
            ImGui::SliderInt("##", &pwmValue, 0, 255);

            static int32_t prevValue = 0;

            if (prevValue != pwmValue)
                SendPWMCommand();

            prevValue = pwmValue;

            static float uD = 0.0f, uQ = 0.0f, uDprev = 0.0f, uQprev = 0.0f;

            ImGui::Text("D voltage setpoint: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            if (ImGui::InputFloat("##uD", &uD, 0.1f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (uDprev != uD) // PROBAR ESTO!!!!!!!!!
                {
                    snprintf(commandStr, RTPLOT_MSG_SIZE, "buD:%.2ff;", uD);
                    *sendCommand = true;
                    uDprev = uD;
                }
            }
            ImGui::SameLine();
            ImGui::Text("Q voltage setpoint: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            if (ImGui::InputFloat("##uQ", &uQ, 0.1f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (uQprev != uQ)
                {
                    snprintf(commandStr, RTPLOT_MSG_SIZE, "buQ:%.2ff;", uQ);
                    *sendCommand = true;
                    uQprev = uQ;
                }
            }

        ImGui::EndChild();
    }

    void InverterControl::Draw(void)
    {
        ImGui::Begin("Inverter Control");
            switch(state)
            {
                case InverterState::none:
                    StartButton();
                    ImGui::SameLine();
                    ResetButton();
                    break;

                case InverterState::waiting:
                    SixStepButton();
                    ImGui::SameLine();
                    SPWMButton();
                    ImGui::SameLine();
                    ResetButton();
                    break;

                case InverterState::sixStep:
                    StopButton();
                    ImGui::SameLine();
                    SixStepOperation();
                    break;

                case InverterState::spwm:
                    StopButton();

                    SPWMOperation();
                    break;

                case InverterState::svpwm:
                    StopButton();
                    break;

                default: 
                    break;
            }

            log.ShowConsoleLog(logMsg, NULL, true, true);
        ImGui::End();
    }
}
