#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include <cmath>
#include <cstdint>
#include <string>

#include <ScrollingBuffer.h>
#include <RollingBuffer.h>

namespace RTPlot
{
	class RealTimePlot
	{
        RollingBuffer* rdata;
        std::string*   writingPtr;
        Graphics*      graphicsPtr;
        double*        dataPtr;
        uint8_t        id = 0;
        ImVec4         plotColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        float          history = 10.0f;
        bool           plotExitFlag = true;

    public:
        RealTimePlot(void);
        RealTimePlot(double* readingPtr, std::string* writingPtr, Graphics* graphicsPtr);
        ~RealTimePlot(void);

        // Getters
        double* GetDataPtr(void)            { return dataPtr; }
        bool    GetPlotExitFlag(void) const { return plotExitFlag; }

        // Setters
        void SetDataPtr(double* ptr) { dataPtr = ptr; }
        void SetID     (uint8_t _id) { id = _id; }

        // Actions
        int8_t Plot (const std::string& name, bool* plotFlag);
        void   Clear(void);
	};
}

#endif