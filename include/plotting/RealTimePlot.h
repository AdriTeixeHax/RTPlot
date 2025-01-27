#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

#include <plotting/ScrollingBuffer.h>
#include <plotting/RollingBuffer.h>
#include <plotting/ColorPalette.h>

namespace RTPlot
{
	class RealTimePlot
	{
        std::vector<RollingBuffer*>  rdata;
        std::vector<ColorPalette*>   plotColors;
        Graphics*      graphicsPtr;
        uint8_t        id = 0;
        float          history = 10.0f;
        bool           exitFlag = true;
        bool           plotExitFlag = false;

    public:
        RealTimePlot(void) = delete;
        RealTimePlot(Graphics* graphicsPtr);
        ~RealTimePlot(void);

        // Getters
        bool GetPlotExitFlag(void) const { return exitFlag; }

        // Setters
        void SetDataToPlot(const std::vector<double>& data);
        void SetID     (uint8_t _id) { id = _id; }

        // Actions
        int8_t Plot (const std::string& name, bool* killFlag);
	};
}

#endif