#ifndef _ROLLINGBUFFER__H_
#define _ROLLINGBUFFER__H_

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

namespace RTPlot // Had to rename the namespace because of linker issues
{
    class RollingBuffer // Taken from implot_demo.cpp
    {
    public:
        double span;
        ImVector<ImVec2> data;

        RollingBuffer(void);

        void AddPoint(double x, double y);
    };
}

#endif