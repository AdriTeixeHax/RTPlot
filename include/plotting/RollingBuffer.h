#ifndef _ROLLINGBUFFER__H_
#define _ROLLINGBUFFER__H_

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <string>

namespace RTPlot // Had to rename the namespace because of linker issues (honestly idfk wtf was going on there)
{
    struct RollingBuffer // Taken and modified from implot_demo.cpp
    {
        double span;
        ImVector<ImVec2> data;
        std::string name;
        char tempName[32];

        RollingBuffer(void) = delete;
        RollingBuffer(const std::string& _name);
        ~RollingBuffer(void);

        void AddPoint(double x, double y);
    };
}

#endif