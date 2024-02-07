#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	ScrollingBuffer::ScrollingBuffer(int max_size)
    {
        maxSize = max_size;
        offset = 0;
        data.reserve(maxSize);
    }

    void ScrollingBuffer::addPoint(float x, float y)
    {
        if (data.size() < maxSize)
            data.push_back(ImVec2(x, y));
        else
        {
            data[offset] = ImVec2(x, y);
            offset = (offset + 1) % maxSize;
        }
    }

    void ScrollingBuffer::erase(void)
    {
        if (data.size() > 0)
        {
            data.shrink(0);
            offset = 0;
        }
    }

    RollingBuffer::RollingBuffer(void)
    {
        span = 10.0f;
        data.reserve(2000);
    }

    void RollingBuffer::addPoint(double x, double y)
    {
        double xmod = fmod(x, span);
        if (!data.empty() && xmod < data.back().x)
            data.shrink(0);
        data.push_back(ImVec2(xmod, y));
    }

    int8_t RealTimePlot::plot(void)
    {
        static float t = 0;
        uint8_t misc_flags = 0;

        t += ImGui::GetIO().DeltaTime;
        rdata->addPoint(t, *dataPtr);

        rdata->span = history;

        static ImPlotAxisFlags linePlotFlags = ImPlotAxisFlags_NoTickLabels;
        if (ImPlot::BeginPlot(std::string("Serial Data " + std::to_string(id)).c_str(), ImVec2(-1, 300)))
        {
            ImPlot::SetupAxes("Time [s]", "Value", linePlotFlags, 0);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 2);
            ImPlot::PushStyleColor(ImPlotCol_Line, color);
            ImPlot::PlotLine(std::string("Data stream " + std::to_string(id)).c_str(), &rdata->data[0].x, &rdata->data[0].y, rdata->data.size(), 0, 0, 2 * sizeof(float));
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
        bool open_popup = ImGui::ColorButton(std::string("Color " + std::to_string(id)).c_str(), color, misc_flags);
        if (open_popup)
        {
            ImGui::OpenPopup(std::string("Picker " + std::to_string(id)).c_str());
            backup_color = color;
        }
        if (ImGui::BeginPopup(std::string("Picker " + std::to_string(id)).c_str()))
        {
            ImGui::Text("Pick a color");
            ImGui::Separator();
            ImGui::ColorPicker4(std::string("Pick " + std::to_string(id)).c_str(), (float*)&color, misc_flags | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview);
            ImGui::SameLine();

            ImGui::BeginGroup(); // Lock X position
            ImGui::Text("Current");
            ImGui::ColorButton(std::string("Current " + std::to_string(id)).c_str(), color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40));
            ImGui::Text("Previous");
            if (ImGui::ColorButton(std::string("Previous " + std::to_string(id)).c_str(), backup_color, ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_AlphaPreviewHalf, ImVec2(60, 40)))
                color = backup_color;
            ImGui::Separator();
            ImGui::Text("Palette");
            for (int n = 0; n < IM_ARRAYSIZE(saved_palette); n++)
            {
                ImGui::PushID(n);
                if ((n % 8) != 0)
                    ImGui::SameLine(0.0f, ImGui::GetStyle().ItemSpacing.y);

                ImGuiColorEditFlags palette_button_flags = ImGuiColorEditFlags_NoAlpha | ImGuiColorEditFlags_NoPicker | ImGuiColorEditFlags_NoTooltip;
                if (ImGui::ColorButton(std::string("Palette " + std::to_string(id)).c_str(), saved_palette[n], palette_button_flags, ImVec2(20, 20)))
                    color = ImVec4(saved_palette[n].x, saved_palette[n].y, saved_palette[n].z, color.w); // Preserve alpha!

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

        return 0;
    }

    void RealTimePlot::clear(void)
    {
        rdata = new RollingBuffer;
    }
}