#ifndef _LOGGER__H_
#define _LOGGER__H_

#include <string>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

// Extracted from the imgui_demo.cpp example
namespace ImGui
{
    class Log
    {
    public:
        ImGuiTextBuffer Buf;
        ImGuiTextFilter Filter;
        ImVector<int>   LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
        bool            AutoScroll;

        Log();
        void Clear();
        void Draw(const char* title, bool* p_open = NULL);
        void AddLog(const char* fmt, ...);
    };
}

namespace RTPlot
{
    inline void ShowConsoleLog(const std::string& logMsg, bool* closable)
    {
        static ImGui::Log log;

        ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
        ImGui::Begin("Console output", closable);
        log.AddLog(logMsg.c_str());
        ImGui::End();

        log.Draw("Console output", closable);
    }
}

#endif