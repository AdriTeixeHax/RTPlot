#include <plotting/RealTimePlot.h>

namespace RTPlot
{
    RealTimePlot::RealTimePlot(Graphics* graphicsPtr) : 
        graphicsPtr(graphicsPtr)
    {
        for (size_t i = 0; i < RTPLOT_DATA_NUM - 1; i++)
        {
            rdata.push_back(new RollingBuffer(std::to_string(i)));
            plotColors.push_back(new ColorPalette((ImVec4)ImColor::HSV(i / 7.0f, 1.0f, 1.0f), i));
        }
    }

    RealTimePlot::~RealTimePlot(void)
    {
        for (auto i : rdata) delete i;
    }

    void RealTimePlot::SetDataToPlot(const std::vector<double>& data)
    {
        for (size_t i = 0; i < data.size() - 1; i++)
            rdata.at(i)->AddPoint(data[0], data[i + 1]);
    }

    int8_t RealTimePlot::Plot(const std::string& name)
    {
        ImGui::Begin(name.c_str(), NULL);
            // TODO@Fix
            //ImGui::PushFont(graphicsPtr->GetLargeFontPtr());
            //    for (auto i : rdata)
            //    {
            //        if (!i) break;
            //        ImGui::SameLine();
            //        const float data = i->data.Data->y;
            //        ImGui::Text("Read data: %.2f", data);
            //    }
            //ImGui::PopFont();
       

            for (auto i : rdata)
            {
                i->span = history;
                ImGui::SameLine();
                ImGui::Checkbox(std::string(std::string("Plot var ") + i->name).c_str(), &i->plotFlag);
            }

            ImGui::SliderFloat(std::string("History " + std::to_string(id)).c_str(), &history, 0.1, 10, "%.1f s");

            static ImPlotAxisFlags linePlotFlags = ImPlotAxisFlags_None;
            ImVec2 available_size = ImGui::GetContentRegionAvail();
            if (ImPlot::BeginPlot(std::string("Data " + std::to_string(id)).c_str(), ImVec2(available_size.x, available_size.y - 100)))
            {
                ImPlot::SetupAxes("Time [s]", "Value", linePlotFlags, 0);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2);
                for (uint32_t i = 0; i < rdata.size(); i++)
                {
                    if (rdata.at(i)->plotFlag)
                    {
                        ImPlot::PushStyleColor(ImPlotCol_Line, plotColors.at(i)->GetColor());
                        ImPlot::PlotLine(rdata.at(i)->name.c_str(), &rdata.at(i)->data[0].x, &rdata.at(i)->data[0].y, rdata.at(i)->data.size(), 0, 0, 2 * sizeof(float));
                        plotColors.at(i)->ColorPicker();
                        ImGui::SameLine();
                    }
                }
                ImPlot::EndPlot();
            }
        ImGui::End();

        return 0;
    }
}