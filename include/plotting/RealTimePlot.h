#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include "imgui/imgui.h"
#include "implot/implot.h"

#include <cmath>

namespace ImPlot
{
    class ScrollingBuffer
    {
    public:
        int maxSize;
        int offset;
        ImVector<ImVec2> data;

        ScrollingBuffer(int max_size = 2000)
        {
            maxSize = max_size;
            offset = 0;
            data.reserve(maxSize);
        }

        void addPoint(float x, float y)
        {
            if (data.size() < maxSize)
                data.push_back(ImVec2(x, y));
            else
            {
                data[offset] = ImVec2(x, y);
                offset = (offset + 1) % maxSize;
            }
        }

        void erase(void)
        {
            if (data.size() > 0)
            {
                data.shrink(0);
                offset = 0;
            }
        }
    };

    class RollingBuffer
    {
    public:
        float span;
        ImVector<ImVec2> data;

        RollingBuffer(void)
        {
            span = 10.0f;
            data.reserve(2000);
        }

        void addPoint(float x, float y)
        {
            float xmod = fmodf(x, span);
            if (!data.empty() && xmod < data.back().x)
                data.shrink(0);
            data.push_back(ImVec2(xmod, y));
        }
    };
}

namespace RTPlot
{
	class RealTimePlot
	{
    public:
        void plot(float data)
        {
            static ImPlot::RollingBuffer rdata;
            static float t = 0;
            t += ImGui::GetIO().DeltaTime;

            rdata.addPoint(t, data);

            static float history = 10.0f;
            ImGui::SliderFloat("History", &history, 1, 30, "%.1f s");
            rdata.span = history;

            static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

            if (ImPlot::BeginPlot("##RTPlot", ImVec2(0, 300)))
            {
                ImPlot::SetupAxes(nullptr, nullptr, flags, flags);
                ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
                ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 2000);
                ImPlot::PlotLine("DataStream", &rdata.data[0].x, &rdata.data[0].y, rdata.data.size(), 0, 0, 2 * sizeof(float));
                ImPlot::EndPlot();
            }
        }
	};
}

#endif