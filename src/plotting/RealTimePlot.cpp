#include <plotting/RealTimePlot.h>

namespace RTPlot
{
    RealTimePlot::RealTimePlot(Graphics* graphicsPtr) : 
        graphicsPtr(graphicsPtr)
    {
        for (size_t i = 0; i < RTPLOT_DATA_NUM - 1; i++)
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
        for (size_t i = 0; i < data.size() - 1; i++)
            basicData.at(i).AddPoint(data[0], data[i + 1]);

        for (auto i : plotData)
            i->SetDataToPlot(data);
    }

    int8_t RealTimePlot::Plot(const std::string& name, bool* killFlag, char* command, bool* sendCommand)
    {
        ImGui::Begin(std::string(name + " - Plotting").c_str(), killFlag);
            ImGui::SeparatorText(name.c_str());
            if (ImGui::Button("Add Plot")) plotData.push_back(new PlotData(basicData));
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
            else
                *sendCommand = false;

            std::vector<std::string> currentNames;
            for (uint8_t i = 0; i < basicData.size(); i++)
            {
                currentNames.push_back(basicData[i].name);
                PlotVars(i, name, currentNames);
            }

            for (uint8_t i = 0; i < plotData.size(); i++)
            {
                PlotGraph(i, plotData[i]->GetKillPtr());
                if (ImGui::BeginDragDropTarget())
                {
                    ImGuiDragDropFlags drop_target_flags = ImGuiDragDropFlags_AcceptNoPreviewTooltip;
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("plotDndPayloadSet", drop_target_flags))
                    {
                        for (size_t j = 0; j < plotData[i]->rdata.size(); j++)
                        {
                            if (std::string((char*)payload->Data) == plotData[i]->rdata[j].name)
                                plotData[i]->rdata[j].plotFlag = true;
                        }
                    }
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("plotDndPayloadReset", drop_target_flags))
                    {
                        for (size_t j = 0; j < plotData[i]->rdata.size(); j++)
                        {
                            if (std::string((char*)payload->Data) == plotData[i]->rdata[j].name)
                                plotData[i]->rdata[j].plotFlag = false;
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

    int8_t RealTimePlot::PlotVars(uint8_t i, const std::string& portName, const std::vector<std::string>& currentNames)
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

            ImGui::BeginChild(basicData[i].name.c_str(), ImVec2(availX, 100), ImGuiChildFlags_Border);

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

                ImGui::Button("Drag to plot");
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("plotDndPayloadSet", basicData[i].GetNameCPtr(), sizeof(char)*32);
                    ImGui::Text(std::string("Plot " + basicData[i].name).c_str());
                    ImGui::EndDragDropSource();
                }
                ImGui::SameLine();
                ImGui::Button("Drag to delete");
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("plotDndPayloadReset", basicData[i].GetNameCPtr(), sizeof(char) * 32);
                    ImGui::Text(std::string("Plot " + basicData[i].name).c_str());
                    ImGui::EndDragDropSource();
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
            if (ImGui::Button("Delete this plot")) *killPlotFlag = true;
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

                plotData[id]->PlotGraph(plotColors);

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