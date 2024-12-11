#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <plotting/ScrollingBuffer.h>
#include <plotting/RollingBuffer.h>

namespace RTPlot
{
	class RealTimePlot
	{
        std::vector<double> dataToPlot;
        RollingBuffer* rdata;
        Graphics*      graphicsPtr;
        uint8_t        id = 0;
        ImVec4         plotColor = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
        float          history = 10.0f;
        bool           plotExitFlag = true;

    public:
        RealTimePlot(void) = delete;
        RealTimePlot(Graphics* graphicsPtr);
        ~RealTimePlot(void);

        // Getters
        std::vector<double>& GetDataToPlot(void) { return dataToPlot; }
        bool GetPlotExitFlag(void) const { return plotExitFlag; }

        // Setters
        void SetDataToPlot(std::vector<double>* data);
        void SetID     (uint8_t _id) { id = _id; }

        // Actions
        int8_t Plot (const std::string& name);
        void   Clear(void);
	};
}

#endif