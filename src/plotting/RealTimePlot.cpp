#include <plotting/RealTimePlot.h>

namespace RTPlot
{
    RealTimePlot::RealTimePlot(Graphics* graphicsPtr) : 
        graphicsPtr(graphicsPtr)
    {
        for (size_t i = 0; i < RTPLOT_DATA_NUM - 1; i++)
        {
            basicData.push_back(new RollingBuffer(std::to_string(i)));
            plotColors.push_back(new ColorPalette((ImVec4)ImColor::HSV(i / 7.0f, 1.0f, 1.0f), i));
        }

        plotData.push_back(new PlotData(basicData)); // Initial plot
    }

    RealTimePlot::~RealTimePlot(void)
    {
        for (auto i : plotColors) delete i;
        for (auto i : plotData)   delete i;
        for (auto i : basicData)  delete i;
    }

    void RealTimePlot::SetDataToPlot(const std::vector<double>& data)
    {
        for (size_t i = 0; i < data.size() - 1; i++)
            basicData.at(i)->AddPoint(data[0], data[i + 1]);

        for (auto i : plotData)
            i->SetDataToPlot(basicData);
    }

    int8_t RealTimePlot::Plot(const std::string& name, bool* killFlag)
    {
        ImGui::Begin(name.c_str(), killFlag);
            ImGui::SeparatorText(name.c_str());
            if (ImGui::Button("Add Plot")) plotData.push_back(new PlotData(basicData));

            const float availX = ImGui::GetContentRegionAvail().x - 15;
            std::vector<std::string> currentNames;
            for (uint8_t i = 0; i < basicData.size(); i++)
            {
                currentNames.push_back(basicData[i]->name);
            }

            for (uint8_t i = 0; i < basicData.size(); i++)
            {
                ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_WINDOW_RADIUS);
                ImGui::BeginChild(basicData[i]->name.c_str(), ImVec2(availX / basicData.size(), 200), ImGuiChildFlags_Border);

                    ImGui::SeparatorText(basicData[i]->name.c_str());
                    ImGui::Text("Plot color:");
                    ImGui::SameLine();
                    plotColors.at(i)->ColorPicker();
                    ImGui::SameLine();
                    // create the drag thingy instead of this
                    //ImGui::Checkbox(std::string(std::string("Plot ") + basicData[i]->name).c_str(), &plotData[i]->plotFlag);
                    bool sameNameFlag = false;
                    bool emptyFlag = false;

                    for (auto names : currentNames)
                    {
                        if (basicData[i]->tempName == names)
                            sameNameFlag = true;
                    }

                    if (basicData[i]->tempName[0] == '\0')
                        emptyFlag = true;

                    if (ImGui::InputText(" ", basicData[i]->tempName, sizeof(basicData[i]->tempName), ImGuiInputTextFlags_EnterReturnsTrue))
                    {
                        if (!sameNameFlag && !emptyFlag) basicData[i]->name = basicData[i]->tempName;
                    }
                    ImGui::SameLine();
                    if (ImGui::Button("Change name"))
                    {
                        if (!sameNameFlag && !emptyFlag) basicData[i]->name = basicData[i]->tempName;
                    }                    

                    if (sameNameFlag && basicData[i]->tempName != basicData[i]->name)
                        ImGui::Text("There is already a variable with that name.");

                    if (emptyFlag) 
                        ImGui::Text("You cannot enter an empty variable name.");


                ImGui::EndChild();
                ImGui::PopStyleVar();
                ImGui::SameLine();
            }

            ImGui::NewLine();

            for (uint8_t i = 0; i < plotData.size(); i++)
            {
                PlotGraph(i);
                ImGui::SameLine();
            }                

        ImGui::End();

        return 0;
    }

    int8_t RealTimePlot::PlotGraph(uint8_t id)
    {
        static ImPlotAxisFlags linePlotFlags = ImPlotAxisFlags_None;
        ImVec2 available_size = ImGui::GetContentRegionAvail();
        std::string name = std::to_string(id) + "graph"; // Just to avoid child overlapping
        
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_WINDOW_RADIUS);
        ImGui::BeginChild(name.c_str(), ImVec2(available_size.x, available_size.y), ImGuiChildFlags_Border);
            if (ImPlot::BeginPlot(std::string("Data " + std::to_string(id)).c_str(), ImVec2(available_size.x, available_size.y - 50)))
            {
                ImPlot::SetupAxes("Time [s]", "Value", linePlotFlags, 0);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, *(plotData[id]->history), ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2);

                plotData[id]->PlotGraph(plotColors);

                ImPlot::EndPlot();
            }
            ImGui::Text("History [s]:");
            ImGui::SameLine();
            ImGui::SliderFloat(" ", plotData[id]->history, 0.1, 60, "%.1f s");
        ImGui::EndChild();
        ImGui::PopStyleVar();

        for (uint8_t i = 0; i < plotData[id]->rdata.size(); i++)
            plotData[id]->rdata[i]->span = *plotData[id]->history;

        static float prevHistory = 0.0f;
        if (*plotData[id]->history != prevHistory)
        {
            plotData[id]->rdata.clear();
            prevHistory = *plotData[id]->history;
        }

        return 0;
    }
}