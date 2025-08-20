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
        if (logMsg == "[RTSpeed Conf]: none")      
            state = InverterState::none;    

        else if (logMsg == "[RTSpeed Conf]: waiting")   
            state = InverterState::waiting; 

        else if (logMsg == "[RTSpeed Conf]: sixStep")   
            state = InverterState::sixStep; 

        else if (logMsg == "[RTSpeed Conf]: spwm")      
            state = InverterState::spwm;    

        else if (logMsg == "[RTSpeed Conf]: svpwm")     
            state = InverterState::svpwm;   

        //if (logMsg != "") std::cout << "Inverter Log Msg: " << logMsg << std::endl;
            
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
        if      (logMsg == "[RTSpeed Conf]: noTHIPWM")  { spwmSelect = 0; }
        else if (logMsg == "[RTSpeed Conf]: triTHIPWM") { spwmSelect = 1; }
        else if (logMsg == "[RTSpeed Conf]: sinTHIPWM") { spwmSelect = 2; }
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
        ImVec2 newAvail = avail;

        ProcessSPWMStateChange();

        if (avail.x >= SPWM_MAX_WIDTH) newAvail.x = SPWM_MAX_WIDTH;
        else newAvail.y = avail.y / 2;

        ImGui::BeginChild("SPWM configuration", newAvail, ImGuiChildFlags_Border);
            ImGui::Text("SPWM modifier wave: ");
            if (avail.x >= SPWM_MAX_WIDTH) ImGui::SameLine();
            ImGui::RadioButton("None",                &spwmSelect, 0); 
            if (avail.x >= SPWM_MAX_WIDTH) ImGui::SameLine();
            ImGui::RadioButton("Triangular Modifier", &spwmSelect, 1); 
            if (avail.x >= SPWM_MAX_WIDTH) ImGui::SameLine();
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

            static float uD = 0.0f, uQ = 0.0f, uDprev = 0.0f, uQprev = 0.0f;

            ImGui::Text("D voltage setpoint: ");
            ImGui::SameLine();
            ImGui::SetNextItemWidth(150);
            if (ImGui::InputFloat("##uD", &uD, 0.1f, 0.0f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
            {
                if (uDprev != uD)
                {
                    snprintf(commandStr, RTPLOT_MSG_SIZE, "buD:%.2ff;", uD);
                    *sendCommand = true;
                    uDprev = uD;
                }
            }
            if (avail.x >= SPWM_MAX_WIDTH) ImGui::SameLine();
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

        if (avail.x >= SPWM_MAX_WIDTH) ImGui::SameLine();
    }

    void InverterControl::IVFilteringTweak(void)
    {
        static float iRegValue = 0.1f;
        static float prevIregValue = iRegValue;
        ImVec2 avail = ImGui::GetContentRegionAvail();
        ImGui::Text("Filter adjust");
        ImGui::SameLine();
        ImGui::SetNextItemWidth(150);
        ImGui::InputFloat("##iregValue", &iRegValue, 0.001f, 0.1f, "%.3f", ImGuiInputTextFlags_EnterReturnsTrue);
        
        ImGui::SetNextItemWidth(fmin(avail.x, 500));
        ImGui::SliderFloat("##iregValueSlider", &iRegValue, 0.0f, 50.0f);

        if (prevIregValue != iRegValue)
        {
            snprintf(commandStr, RTPLOT_MSG_SIZE, "biRegValue:%.3ff;", iRegValue);
            *sendCommand = true;
            prevIregValue = iRegValue;
        }
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

                    IVFilteringTweak();
                    break;

                case InverterState::sixStep:
                    StopButton();
                    ImGui::SameLine();
                    SixStepOperation();

                    IVFilteringTweak();
                    break;

                case InverterState::spwm:
                    StopButton();

                    IVFilteringTweak();

                    SPWMOperation();
                    break;

                case InverterState::svpwm:
                    StopButton();

                    IVFilteringTweak();
                    break;

                default: 
                    break;
            }

            log.ShowConsoleLog(logMsg, NULL, true, true);
        ImGui::End();
    }
}
