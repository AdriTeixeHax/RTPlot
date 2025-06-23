#include <Logger.h>
#include <iostream>
#include <cstdint>

ImGui::Log::Log(const std::string& windowName) : name(windowName)
{
    AutoScroll = true;
    Clear();
}

void ImGui::Log::Clear()
{
    Buf.clear();
    LineOffsets.clear();
    LineOffsets.push_back(0);
}

void ImGui::Log::Draw(const char* title, bool showOnlyText, bool childOnly, bool* p_open)
{
    if (!childOnly) ImGui::Begin(title, p_open);
        bool clear = false, copy = false;
        if (!showOnlyText)
        {
            // Options menu
            if (ImGui::BeginPopup("Options"))
            {
                ImGui::Checkbox("Auto-scroll", &AutoScroll);
                ImGui::EndPopup();
            }

            // Main window
            if (ImGui::Button("Options"))
                ImGui::OpenPopup("Options");
            ImGui::SameLine();
            clear = ImGui::Button("Clear");
            ImGui::SameLine();
            copy = ImGui::Button("Copy");
            ImGui::SameLine();
            Filter.Draw("Filter", -100.0f);

            ImGui::Separator();
        }

        if (ImGui::BeginChild("scrolling", ImVec2(0, 0), ImGuiChildFlags_Border, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (clear) Clear();
            if (copy) ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char* buf = Buf.begin();
            const char* buf_end = Buf.end();
            if (Filter.IsActive())
            {
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            }
            else
            {
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char* line_start = buf + LineOffsets[line_no];
                        const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();
            if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    if (!childOnly) ImGui::End();
}

void ImGui::Log::AddLog(const char* fmt, ...)
{
    int old_size = Buf.size();
    va_list args;
    va_start(args, fmt);
    Buf.appendfv(fmt, args);
    va_end(args);
    for (int new_size = Buf.size(); old_size < new_size; old_size++)
        if (Buf[old_size] == '\n')
            LineOffsets.push_back(old_size + 1);
}

void ImGui::Log::ShowConsoleLog(const std::string& logMsg, bool* closable, bool childOnly, bool showOnlyText)
{    
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    if (!childOnly) ImGui::Begin(name.c_str(), closable);
        if (prevMsg != logMsg)
        {
            std::cout << "logMsg:" << std::endl;
            for (uint8_t i = 0; i < 255; i++)
            {
                std::cout << (int)i << ": " << logMsg.c_str()[i] << ", " << (int)logMsg.c_str()[i] << std::endl;
            }

            AddLog(logMsg.c_str());
            prevMsg = logMsg;
        }
    if (!childOnly) ImGui::End();

    Draw(name.c_str(), showOnlyText, childOnly, closable);
}
