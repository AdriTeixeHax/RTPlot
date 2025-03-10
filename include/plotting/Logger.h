/// Logger.h - Header file for the Logger class, used to manage the console log in the ImGui window.

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

        void ShowConsoleLog(const std::string& logMsg, bool* closable);
    };
}

#endif