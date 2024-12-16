#include <plotting/RealTimePlot.h>

namespace RTPlot
{
    RealTimePlot::RealTimePlot(Graphics* graphicsPtr) : 
        graphicsPtr(graphicsPtr)
    { 
        for (size_t i = 0; i < RTPLOT_DATA_NUM - 1; i++)
            rdata.push_back(new RollingBuffer(std::to_string(i)));
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
       
            uint8_t misc_flags = 0;

            for (auto i : rdata)
            {
                i->span = history;
                ImGui::SameLine();
                ImGui::Checkbox(std::string(std::string("Plot var ") + i->name).c_str(), &i->plotFlag);
            }

            static ImPlotAxisFlags linePlotFlags = ImPlotAxisFlags_None;
            if (ImPlot::BeginPlot(std::string("Data " + std::to_string(id)).c_str(), ImVec2(-1, -1)))
            {
                ImPlot::SetupAxes("Time [s]", "Value", linePlotFlags, 0);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, -2, 2);
                ImPlot::PushStyleColor(ImPlotCol_Line, plotColor);
                for (auto i : rdata)
                {
                    if (i->plotFlag)
                        ImPlot::PlotLine(i->name.c_str(), &i->data[0].x, &i->data[0].y, i->data.size(), 0, 0, 2 * sizeof(float));
                }
                ImPlot::EndPlot();
            }
            ImGui::SliderFloat(std::string("History " + std::to_string(id)).c_str(), &history, 0.1, 10, "%.1f s");
            ImGui::SameLine();

            // Generate a default palette. The palette will persist and can be edited.
            static bool saved_palette_init = true;
            static ImVec4 saved_palette[32] = {};
            if (saved_palette_init)
            {
                for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
                {
                    ImGui::ColorConvertHSVtoRGB(n / 31.0f, 0.8f, 0.8f,
                        saved_palette[n].x, saved_palette[n].y, saved_palette[n].z);
                    saved_palette[n].w = 1.0f; // Alpha
                }
                saved_palette_init = false;
            }

            static ImVec4 backup_color;
            bool open_popup = ImGui::ColorButton(std::string("Color " + std::to_string(id)).c_str(), plotColor, misc_flags);
            if (open_popup)
            {
                ImGui::OpenPopup(std::string("Picker " + std::to_string(id)).c_str());
                backup_color = plotColor;
            }
            if (ImGui::BeginPopup(std::string("Picker " + std::to_string(id)).c_str()))
            {
                ImGui::Text("Pick a color");
                ImGui::Separator();
                ImGui::ColorPicker4(std::string("Pick " + std::to_string(id)).c_str(), (float*)&plotColor, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
                ImGui::SameLine();

                ImGui::BeginGroup(); // Lock X position
                ImGui::Text("Current");
                ImGui::ColorButton(std::string("Current " + std::to_string(id)).c_str(), plotColor, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
                ImGui::Text("Previous");
                if (ImGui::ColorButton(std::string("Previous " + std::to_string(id)).c_str(), backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                    plotColor = backup_color;
                ImGui::Separator();
                ImGui::Text("Palette");
                for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
                {
                    ImGui::PushID(n);
                    if ((n % 8) != 0)
                        ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

                    ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                    if (ImGui::ColorButton(std::string("Palette " + std::to_string(id)).c_str(), saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                        plotColor = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, plotColor.w); // Preserve alpha!

                    // Allow user to drop colors into each palette entry. Note that ColorButton() is already a
                    // drag source by default, unless specifying the ImGuiColorEditFlags_NoDragDrop flag.
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_3F))
                            memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 3);
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(IMGUI_PAYLOAD_TYPE_COLOR_4F))
                            memcpy((float*)&saved_palette[n], payload->Data, sizeof(float) * 4);
                        ImGui::EndDragDropTarget();
                    }

                    ImGui::PopID();
                }
                ImGui::EndGroup();
                ImGui::EndPopup();
            }
        ImGui::End();

        return 0;
    }
}