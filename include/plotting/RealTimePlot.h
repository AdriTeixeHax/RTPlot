#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include "imgui/imgui.h"
#include "implot/implot.h"

#include <cmath>
#include <cstdint>
#include <string>

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
        RollingBuffer* rdata;
        double* dataPtr;
        uint8_t id;

        std::string* writingPtr;

        ImVec4 color = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);

        float history = 10.0f;

        bool plotExitFlag = true;

    public:
        RealTimePlot(void) : rdata(new RollingBuffer), dataPtr(new double), id(0), writingPtr(new std::string) { }
        RealTimePlot(double* readingPtr, std::string* writingPtr) : rdata(new RollingBuffer), dataPtr(readingPtr), writingPtr(writingPtr), id(0) { }
        ~RealTimePlot(void) 
        { 
            delete rdata; 
            dataPtr = new double;
            delete dataPtr;
            writingPtr = new std::string;
            delete writingPtr;
        }

        // Getters
        double* getDataPtr(void)   { return dataPtr; }
        bool getPlotExitFlag(void) { return plotExitFlag; }

        // Setters
        void setDataPtr(double* ptr) { dataPtr = ptr; }
        void setID(uint8_t _id) { id = _id; }

        // Actions
        int8_t plot(const std::string& name, bool* plotFlag);
        void clear(void);
	};
}

#endif