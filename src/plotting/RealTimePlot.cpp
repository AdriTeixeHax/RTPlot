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
        static RTPlot::RollingBuffer rdata;
        static float t = 0;

        t += ImGui::GetIO().DeltaTime;
        rdata.addPoint(t, *dataPtr);

        static float history = 10.0f;
        rdata.span = history;

        static ImPlotAxisFlags flags = ImPlotAxisFlags_NoTickLabels;

        if (ImPlot::BeginPlot("##RTPlot", ImVec2(-1, 300)))
        {
            ImPlot::SetupAxes("Time [s]", "Value", flags, 0);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
            ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5, 2);
            ImPlot::PlotLine("DataStream", &rdata.data[0].x, &rdata.data[0].y, rdata.data.size(), 0, 0, 2 * sizeof(float));
            ImPlot::EndPlot();
        }
        ImGui::SliderFloat("History", &history, 0.1, 10, "%.1f s");

        return 0;
    }
}