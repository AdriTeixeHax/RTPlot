/// RollingBuffer.h - Class to manage a buffer that scrolls after reaching a certain size or value.

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
        bool plotFlag = false;

        RollingBuffer(void) = delete;
        RollingBuffer(const std::string& _name);
        ~RollingBuffer(void);

        RollingBuffer operator=(const RollingBuffer& rbuf)
        {
            RollingBuffer result(rbuf.name);
            result.data = rbuf.data;
            strcpy_s(result.tempName, rbuf.tempName);
            result.data.reserve(2000);
            return result;
        }

        char* GetNameCPtr(void) { static char cname[32]; strcpy_s(cname, name.c_str()); return cname; }

        void AddPoint(double x, double y);
    };
}

#endif