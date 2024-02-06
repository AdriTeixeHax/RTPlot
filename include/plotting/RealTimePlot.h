#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include "imgui/imgui.h"
#include "implot/implot.h"

#include <cmath>
#include <cstdint>

namespace RTPlot
{
    class ScrollingBuffer // Taken from implot_demo.cpp
    {
    public:
        int maxSize;
        int offset;
        ImVector<ImVec2> data;

        ScrollingBuffer(int max_size = 2000);

        void addPoint(float x, float y);
        void erase(void);
    };

    class RollingBuffer // Taken from implot_demo.cpp
    {
    public:
        double span;
        ImVector<ImVec2> data;

        RollingBuffer(void);

        void addPoint(double x, double y);
    };

	class RealTimePlot
	{
        double* dataPtr = nullptr;

    public:
        RealTimePlot(void) : dataPtr(nullptr) { }
        RealTimePlot(double* ptr) : dataPtr(ptr) { }

        // Getters
        double* getDataPtr(void) { return dataPtr; }

        // Setters
        void setDataPtr(double* ptr) { dataPtr = ptr; }

        // Actions
        int8_t plot(void);
	};
}

#endif