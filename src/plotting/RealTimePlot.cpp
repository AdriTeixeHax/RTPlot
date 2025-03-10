#include <plotting/RealTimePlot.h>

#include <../src/vendor/implot/implot.h>

#include <thread>
#include <iostream>

#include <RTPlotVars.h>
#include <RTPlotFunctions.h>

namespace RTPlot
{
    RealTimePlot::RealTimePlot(void)
    {
        for (uint8_t i = 0; i < RTPLOT_MAX_DATA_NUM - 1; i++) // 1 from time
        {
            basicData.push_back(new PlotData(i, std::to_string(i).c_str()));
        }

        plotters.push_back(new Plotter(&basicData)); // Initial plot
    }

    RealTimePlot::~RealTimePlot(void)
    {
        for (auto i : plotters) delete i;
        for (auto i : basicData) delete i;
    }

    void RealTimePlot::SetDataToPlot(const std::vector<double>& originalData)
    {
        if (originalData.size() < 2) { std::cerr << "[RealTimePlot]: Error setting data to plot." << std::endl; return; }

        for (size_t i = 0; i < basicData.size() - 1; i++)
            basicData.at(i)->plotData->AddPoint(originalData[0], originalData[i + 1]);

        for (auto i : plotters)
			i->SetDataToPlot(originalData);
    }

    int8_t RealTimePlot::Plot(const std::string& name, bool* killFlag, char* command, bool* sendCommand, bool* addVariable, uint32_t* varToRemove, bool* removeVariable)
    {
        // Plotting window
        ImGui::Begin(std::string(name + " - Plotting").c_str(), killFlag);
            // Header text    
            ImGui::SeparatorText(name.c_str());

            // "Add plot" button
            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
            if (ImGui::Button("Add Plot"))
                plotters.push_back(new Plotter(&basicData));
            ImGui::PopStyleColor(3);
            ImGui::SameLine();

			// Send command to device, either by pressing enter or the "Send" button.
            static char tempCommand[RTPLOT_MSG_SIZE] = "Send message to device";
			static bool sendFlag = false;
            ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 48);
            if (ImGui::InputText(std::string("##" + name).c_str(), tempCommand, sizeof(tempCommand), ImGuiInputTextFlags_EnterReturnsTrue))
                sendFlag = true;
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("Send") || sendFlag)
            {
                strcpy_s(command, sizeof(tempCommand), tempCommand);
                *sendCommand = true;
                sendFlag = false;
            }

            /// BEGIN Test environmet for PI controller
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
            /// END test environment

            // Plot graphs for all plots
            for (uint8_t i = 0; i < plotters.size(); i++)
            {
                PlotGraph(i, plotters.at(i)->GetKillPtr());
                if (ImGui::BeginDragDropTarget())
                {
                    // Read payload
                    ImGuiDragDropFlags dropTargetFlags = ImGuiDragDropFlags_AcceptNoPreviewTooltip;
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("plotDndPayload", dropTargetFlags))
                    {
                        for (size_t j = 0; j < plotters.at(i)->GetDataSize(); j++)
                        {
							// If the name sent is the same as the one in the plotter, toggle the plot flag
                            std::string payloadName = std::string((char*)payload->Data);
                            std::string varName = plotters.at(i)->GetDataPtr()->at(j)->dataName;
                            if (payloadName == varName)
                            {
                                bool& plotFlag = plotters.at(i)->GetDataPtr()->at(j)->plotData->GetPlotFlagRef();
                                RTPlot::Toggle(plotFlag);
                                break;
                            }
                        }
                    }
                    ImGui::EndDragDropTarget();
                }
                ImGui::SameLine();
                
				// Delete element if its kill flag is true
                if (*plotters.at(i)->GetKillPtr() == true)
                    plotters.erase(plotters.begin() + i);
            }                

            // Variable modification window. Lives inside the other window so that it can close with it.
            ImGui::Begin(std::string(name + " - Data settings").c_str(), NULL);
                // Number of variables visible and text flag
                static bool startTimeFlag = true;
                static bool buttonMsgFlag = false;
                static std::string buttonMsg;
            
                // "Add variable" button
                ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.35f, 1.0f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.35f, 0.7f, 0.5f));
                if (ImGui::Button("Add variable"))
                {
                    if (visibleVarsNum < RTPLOT_MAX_DATA_NUM - 1)
                    {
                        basicData.at(visibleVarsNum)->plottable = true;

                        for (auto i : plotters)
                            i->GetDataPtr()->at(visibleVarsNum)->plottable = true;

                        visibleVarsNum++;
                    }
                    else
                    {
                        startTimeFlag = true;
                        buttonMsgFlag = true;
                        buttonMsg = "Max. no. of variables reached!";
                    }
                }
                ImGui::PopStyleColor(3);

                // "Remove variable" button
                ImGui::SameLine();
                ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.5f));
                if (ImGui::Button("Remove variable"))
                {
                    if (visibleVarsNum > 0)
                    {
                        basicData.at(visibleVarsNum - 1)->plottable = false;

                        for (auto i : plotters)
                            i->GetDataPtr()->at(visibleVarsNum - 1)->plottable = false;

                        visibleVarsNum--;
                    }
                    else
                    {
                        startTimeFlag = true;
                        buttonMsgFlag = true;
                        buttonMsg = "There are no variables to remove!";
                    }
                }
                ImGui::PopStyleColor(3);

                // Delayed message
                if (buttonMsgFlag)
                {
                    static auto startTime = std::chrono::steady_clock::now();

                    if (startTimeFlag == true)
                    {
                        startTime = std::chrono::steady_clock::now();
                        startTimeFlag = false;
                    }

                    auto elapsed = (std::chrono::steady_clock::now() - startTime).count() / 1e6;

                    if (elapsed <= 1000) // 1 second
                    {
                        ImGui::SameLine();
                        ImGui::Text(buttonMsg.c_str());
                    }
                    else buttonMsgFlag = false;
                }

                // Plot variable settings
                std::vector<std::string> currentNames;
                for (uint8_t i = 0; i < plotters.at(0)->GetDataPtr()->size(); i++)
                {
                    PlotData* data = plotters.at(0)->GetDataPtr()->at(i);

                    currentNames.push_back(data->dataName);

                    if (data->plottable)
                        PlotVars(i, name, currentNames, command, sendCommand);
                }
            ImGui::End();

        ImGui::End();

        return 0;
    }

    int8_t RealTimePlot::PlotVars(uint8_t i, const std::string& portName, const std::vector<std::string>& currentNames, char* command, bool* sendCommand)
    {
		if (plotters.size() == 0) return 0;

        bool emptyFlag    = false;
        bool sameNameFlag = false;

		// Create a copy of the temp name and the current name variables.
		char*       tempName    = plotters.at(0)->GetDataPtr()->at(i)->tempDataName;
		std::string currentName = plotters.at(0)->GetDataPtr()->at(i)->dataName;

        for (auto names : currentNames)
        {
            if (tempName == names)
                sameNameFlag = true;
        }

        if (tempName[0] == '\0')
            emptyFlag = true;

        ImGui::Begin(std::string(portName + " - Data settings").c_str(), NULL);
            const float availX = ImGui::GetContentRegionAvail().x;
            const float availY = ImGui::GetContentRegionAvail().y;
            static bool dragFlag = false;

            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_WINDOW_RADIUS);

            ImGui::BeginChild(currentName.c_str(), ImVec2(availX, 100), ImGuiChildFlags_Border);
                if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
                {
                    ImGui::SetDragDropPayload("plotDndPayload", currentName.c_str(), sizeof(char) * strlen(currentName.c_str()));
                    ImGui::Text(std::string("Plot/Remove " + currentName).c_str());
                    ImGui::EndDragDropSource();
                }

                ImGui::SeparatorText(currentName.c_str());

                ImGui::Text("Plot color:");
                ImGui::SameLine();

                plotters.at(0)->ColorPicker(i);
                ImGui::SameLine();

                ImGui::PushItemWidth(availX - 216);
                if (ImGui::InputText("##", tempName, sizeof(tempName), ImGuiInputTextFlags_EnterReturnsTrue))
                {
                    if (!sameNameFlag && !emptyFlag)
                    {
                        currentName = tempName;
                        for (auto k : plotters)
                            k->GetDataPtr()->at(i)->dataName = tempName;
                    }
                }
                ImGui::PopItemWidth();
                ImGui::SameLine();

                if (ImGui::Button("Change name"))
                {
                    if (!sameNameFlag && !emptyFlag)
                    {
                        for (auto k : plotters)
                            k->GetDataPtr()->at(i)->dataName = tempName;
                    }
                }

                if (sameNameFlag && tempName != currentName)
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
        // Get region dimensions
        const float availX = ImGui::GetContentRegionAvail().x / (plotters.size() - id);
        const float availY = ImGui::GetContentRegionAvail().y;

        // Begin child window with style
        static ImPlotAxisFlags linePlotFlags = ImPlotAxisFlags_None;
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_WINDOW_RADIUS);
        ImGui::BeginChild(std::string(std::to_string(id) + "graph").c_str(), ImVec2(availX, availY), ImGuiChildFlags_Border);
            // Delete plot button
            ImGui::PushID(id);
            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(0.0f, 1.0f, 0.6f));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.7f));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(0.0f / 10.0f, 0.7f, 0.5f));
            if (ImGui::Button("Delete this plot")) *killPlotFlag = true;
            ImGui::PopStyleColor(3);
            ImGui::PopID();

			// Change plot name
            ImGui::SameLine();
            ImGui::PushItemWidth(availX - 217);
            if (ImGui::InputText("##", plotters.at(id)->GetTempName(), strlen(plotters.at(id)->GetTempName()), ImGuiInputTextFlags_EnterReturnsTrue))
            {
                *(plotters.at(id)->GetNamePtr()) = plotters.at(id)->GetTempName();
            }
            ImGui::PopItemWidth();
            ImGui::SameLine();
            if (ImGui::Button("Change name"))
            {
                *(plotters.at(id)->GetNamePtr()) = plotters.at(id)->GetTempName();
            }

			// Plot graph
            if (ImPlot::BeginPlot(std::string(plotters.at(id)->GetName() + "##" + std::to_string(id)).c_str(), ImVec2(availX - 15, availY - 80)))
            {
                ImPlot::SetupAxes("Time [s]", "Value", linePlotFlags, 0);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, *(plotters.at(id)->GetHistoryPtr()), ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2);

                plotters.at(id)->PlotGraph();

                ImPlot::EndPlot();
            }

			// History slider
            ImGui::Text("History:");
            ImGui::SameLine();
            ImGui::PushItemWidth(availX - 70);
            ImGui::SliderFloat(std::string("##" + std::to_string(id)).c_str(), plotters.at(id)->GetHistoryPtr(), 0.1, 60, "%.1f s");
            ImGui::PopItemWidth();
        ImGui::EndChild();
        ImGui::PopStyleVar();

        for (uint8_t i = 0; i < plotters.at(id)->GetDataSize(); i++)
        {
            float     span     = plotters.at(id)->GetHistory();
            PlotData* plotData = plotters.at(id)->GetDataPtr()->at(i);

            plotData->SetSpan(span);
        }

        return 0;
    }
}