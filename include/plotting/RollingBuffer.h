/// RollingBuffer.h - Class to manage a buffer that scrolls after reaching a certain size or value.

#ifndef _ROLLINGBUFFER__H_
#define _ROLLINGBUFFER__H_

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <string>
#include <vector>

#include <nlohmann/json.hpp>
using JSON = nlohmann::json;

namespace RTPlot // Had to rename the namespace because of linker issues (honestly idfk wtf was going on there)
{
    class RollingBuffer // Taken and modified from implot_demo.cpp
    {
        ImVector<ImVec2> data;
        double           span;
        bool             plotFlag = false;

    public:
        RollingBuffer(void);
        ~RollingBuffer(void);
        RollingBuffer& operator=(const RollingBuffer& rbuf);

        // Getters
		bool              GetPlotFlag   (void)         { return plotFlag; }
		bool&             GetPlotFlagRef(void)         { return plotFlag; }
		size_t            GetDataSize   (void) const   { return data.size(); }
		ImVector<ImVec2>* GetDataPtr    (void)         { return &data; }
		ImVector<ImVec2>& GetDataRef    (void)         { return data; }
        double            GetSpan       (void)         { return span; }

        // Setters
		void              SetSpan       (double _span) { span = _span; }

        // Actions
        void              AddPoint      (double x, double y);
        JSON              toJSON        (void);
        void              fromJSON      (const JSON& j);
    };
}

#endif