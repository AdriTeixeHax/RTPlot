#include <plotting/RealTimePlot.h>

#include <thread>

namespace RTPlot
{
    RealTimePlot::RealTimePlot(Graphics* graphicsPtr) :
        graphicsPtr(graphicsPtr)
    {
        for (uint8_t i = 0; i < RTPLOT_MAX_DATA_NUM - 1; i++)
        {
            basicData.push_back(std::to_string(i));
            plotColors.push_back(new ColorPalette((ImVec4)ImColor::HSV(i / 7.0f, 1.0f, 1.0f), i));
        }

        plotData.push_back(new PlotData(basicData)); // Initial plot
    }

    RealTimePlot::~RealTimePlot(void)
    {
        for (auto i : plotColors) delete i;
        for (auto i : plotData)   delete i;
    }

    void RealTimePlot::SetDataToPlot(const std::vector<double>& data)
    {
        if (dataNum > data.size() || data.size() < 2) return;

        for (size_t i = 0; i < dataNum - 1; i++)
            basicData.at(i).AddPoint(data[0], data[i + 1]);

        for (auto i : plotData)
            i->SetDataToPlot(basicData);
    }

    int8_t RealTimePlot::Plot(const std::string& name, bool* killFlag, char* command, bool* sendCommand, bool* addVariable, uint32_t* varToRemove, bool* removeVariable)
    {
        ImGui::Begin(std::string(name + " - Plotting").c_str(), killFlag);
            ImVec2 nameSize = ImGui::CalcTextSize(name.c_str());
            ImGui::SeparatorText(name.c_str());

            ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
            if (ImGui::Button("Add Plot")) plotData.push_back(new PlotData(basicData));
            ImGui::PopStyleColor(3);

            ImGui::SameLine();

            static char tempCommand[RTPLOT_MSG_SIZE] = "Send message to device";
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 48);
            if (ImGui::InputText(std::string("##" + name).c_str(), tempCommand, sizeof(tempCommand), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                strcpy_s(command, sizeof(tempCommand), tempCommand);
                *sendCommand = true;
            }
            else
                *sendCommand = false;
            ImGui::PopItemWidth();
            ImGui::SameLine();

            if (ImGui::Button("Send"))
            {
                strcpy_s(command, sizeof(tempCommand), tempCommand);
                *sendCommand = true;
            }

            ImGui::Begin(std::string(name + " - Data settings").c_str(), NULL);
                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
                if (ImGui::Button("Add variable"))
                {
					if (dataNum < RTPLOT_MAX_DATA_NUM - 2) // IDK why 2 honestly
						dataNum++;
                }
				ImGui::PopStyleColor(3);
				ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.5f));
                if (ImGui::Button("Remove variable"))
                {
                    if (dataNum > 1) // 1 must be present (time)
                    {
                        for (size_t i = 0; i < plotData.size(); i++)
                        {
                            plotData[i]->rdata[dataNum].plotFlag = !plotData[i]->rdata[dataNum].plotFlag;
                        }
                        dataNum--;
                    }
                }
                ImGui::PopStyleColor(3);
            ImGui::End();

            std::vector<std::string> currentNames;
            if (dataNum > basicData.size() - 1) return RTPLOT_ERROR;
            for (uint8_t i = 0; i < dataNum - 1; i++)
            {
                currentNames.push_back(basicData[i].name);
                PlotVars(i, name, currentNames, command, sendCommand);
            }

            static float kiVal = 0, kpVal = 0;

            ImGui::InputFloat("Ki", &kiVal);
            ImGui::InputFloat("Kp", &kpVal);
            ImGui::SameLine();
            if (ImGui::Button("Apply"))
            {
                char tempCommand[32] = "";
                snprintf(tempCommand, sizeof(tempCommand), "bKi:%.3f;", kiVal);
                strcpy_s(command, sizeof(tempCommand), tempCommand);
                *sendCommand = true;

                while (*sendCommand) std::this_thread::yield();

                snprintf(tempCommand, sizeof(tempCommand), "bKp:%.3f;", kpVal);
                strcpy_s(command, sizeof(tempCommand), tempCommand);
                *sendCommand = true;
            }

            for (uint8_t i = 0; i < plotData.size(); i++)
            {
                PlotGraph(i, plotData[i]->GetKillPtr());
                if (ImGui::BeginDragDropTarget())
                {
                    ImGuiDragDropFlags drop_target_flags = ImGuiDragDropFlags_AcceptNoPreviewTooltip;
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("plotDndPayload", drop_target_flags))
                    {
                        for (size_t j = 0; j < plotData[i]->rdata.size(); j++)
                        {
                            std::string tempNameData = std::string((char*)payload->Data);
                            if (tempNameData == plotData[i]->rdata[j].name)
                            {
                                plotData[i]->rdata[j].plotFlag = !plotData[i]->rdata[j].plotFlag;
                                break;
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                
                if (*plotData[i]->GetKillPtr() == true)
                    plotData.erase(plotData.begin() + i);
            }                

        ImGui::End();

        return 0;
    }

    int8_t RealTimePlot::PlotVars(uint8_t i, const std::string& portName, const std::vector<std::string>& currentNames, char* command, bool* sendCommand)
    {
        bool sameNameFlag = false;
        bool emptyFlag = false;

        for (auto names : currentNames)
        {
            if (basicData[i].tempName == names)
                sameNameFlag = true;
        }

        if (basicData[i].tempName[0] == '\0')
            emptyFlag = true;

        ImGui::Begin(std::string(portName + " - Data settings").c_str(), NULL);
            const float availX = ImGui::GetContentRegionAvail().x;
            const float availY = ImGui::GetContentRegionAvail().y;

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_WINDOW_RADIUS);

            static bool dragFlag = false;
            ImGui::BeginChild(basicData[i].name.c_str(), ImVec2(availX, 100), ImGuiChildFlags_Border);
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("plotDndPayload", basicData[i].GetNameCPtr(), sizeof(char) * 32);
                    ImGui::Text(std::string("Plot/Remove " + basicData[i].name).c_str());
                    ImGui::EndDragDropSource();
                }

                ImGui::SeparatorText(basicData[i].name.c_str());

                ImGui::Text("Plot color:");
                ImGui::SameLine();

                plotColors.at(i)->ColorPicker();
                ImGui::SameLine();

                ImGui::PushItemWidth(availX - 216);
                if (ImGui::InputText("##", basicData[i].tempName, sizeof(basicData[i].tempName), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (!sameNameFlag && !emptyFlag)
                    {
                        basicData[i].name = basicData[i].tempName;
                        for (auto k : plotData)
                            k->rdata[i].name = basicData[i].tempName;
                    }
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button("Change name"))
                {
                    if (!sameNameFlag && !emptyFlag)
                    {
                        basicData[i].name = basicData[i].tempName;
                        for (auto k : plotData)
                            k->rdata[i].name = basicData[i].tempName;
                    }
                }

                if (sameNameFlag && basicData[i].tempName != basicData[i].name)
                    ImGui::Text("There is already a variable with that name.");

                if (emptyFlag)
                    ImGui::Text("You cannot enter an empty variable name.");

            ImGui::EndChild();
            ImGui::PopStyleVar();
        ImGui::End();
        return 0;
    }

    int8_t RealTimePlot::PlotGraph(uint8_t id, bool* killPlotFlag)
    {
        const float availX = ImGui::GetContentRegionAvail().x / (plotData.size() - id);
        const float availY = ImGui::GetContentRegionAvail().y;
        static ImPlotAxisFlags linePlotFlags = ImPlotAxisFlags_None;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_WINDOW_RADIUS);
        ImGui::BeginChild(std::string(std::to_string(id) + "graph").c_str(), ImVec2(availX, availY), ImGuiChildFlags_Border);
            ImGui::PushID(id);
            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.5f));
        
            if (ImGui::Button("Delete this plot")) *killPlotFlag = true;

            ImGui::PopStyleColor(3);
            ImGui::PopID();

            ImGui::SameLine();
            ImGui::PushItemWidth(availX - 217);
            if (ImGui::InputText("##", plotData[id]->tempName, sizeof(plotData[id]->tempName), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                plotData[id]->name = plotData[id]->tempName;
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("Change name"))
            {
                plotData[id]->name = plotData[id]->tempName;
            }

            if (ImPlot::BeginPlot(std::string(plotData[id]->GetName() + "##" + std::to_string(id)).c_str(), ImVec2(availX - 15, availY - 80)))
            {
                ImPlot::SetupAxes("Time [s]", "Value", linePlotFlags, 0);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, *(plotData[id]->history), ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2);

                plotData[id]->PlotGraph(plotColors, dataNum);

                ImPlot::EndPlot();
            }
            ImGui::Text("History:");
            ImGui::SameLine();

            ImGui::PushItemWidth(availX - 70);
            ImGui::SliderFloat(std::string("##" + std::to_string(id)).c_str(), plotData[id]->history, 0.1, 60, "%.1f s");
            ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::PopStyleVar();

        for (uint8_t i = 0; i < plotData[id]->rdata.size(); i++)
            plotData[id]->rdata[i].span = *plotData[id]->history;

        return 0;
    }
}