#include <../../include/plotting/ColorPalette.h>

#include <string>

RTPlot::ColorPalette::ColorPalette(const ImVec4& _color, uint32_t _id) :
    color(_color), id(_id) { }

void RTPlot::ColorPalette::ColorPicker(void)
{
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
    uint8_t misc_flags = 0;

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
}

JSON RTPlot::ColorPalette::toJSON(void)
{
    return 
    {
        {"id", id},
        {"color", Vec4ToJSON(color)}
    };
}

void RTPlot::ColorPalette::fromJSON(const JSON& j)
{
    j.at("id").get_to(id);
    JSONToVec4(j.at("color"), color);
}
