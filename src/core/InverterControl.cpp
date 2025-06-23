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

    void InverterControl::Draw(void)
    {
        ImGui::Begin("Inverter Control");
            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
            if (ImGui::Button("Start inverter"))
            {
                strcpy(commandStr, "RTSPEED_START");
                *sendCommand = true;
            }
            ImGui::PopStyleColor(3);    
            
            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
            if (ImGui::Button("Start 6 Step Algorithm"))
            {
                strcpy(commandStr, "RTSPEED_BEGIN_6STEP");
                *sendCommand = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
            if (ImGui::Button("Start SPWM"))
            {
                strcpy(commandStr, "RTSPEED_BEGIN_SPWM");
                spwmFlag = true;
                *sendCommand = true;
            }
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.5f));
            if (ImGui::Button("Stop inverter"))
            {
                strcpy(commandStr, "RTSPEED_STOP");
                *sendCommand = true;
            }
            ImGui::PopStyleColor(3);

            if (spwmFlag)
            {
                ImVec2 avail = ImGui::GetContentRegionAvail();
                ImGui::BeginChild("SPWM configuration", ImVec2(avail.x, 42), ImGuiChildFlags_Border);
                    ImGui::RadioButton("None", &spwmSelect, 0);
                    ImGui::SameLine();
                    ImGui::RadioButton("Triangular SuperSinusoids", &spwmSelect, 1);
                    ImGui::SameLine();
                    ImGui::RadioButton("Sinusoidal SuperSinusoids", &spwmSelect, 2);

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
                ImGui::EndChild();
            }

            log.ShowConsoleLog(logMsg, NULL, true, true);
        ImGui::End();
    }
}
