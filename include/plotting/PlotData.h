#ifndef _PLOTDATA__H_
#define _PLOTDATA__H_

#include <vector>

#include <plotting/ScrollingBuffer.h>
#include <plotting/RollingBuffer.h>
#include <plotting/ColorPalette.h>

namespace RTPlot
{
    struct PlotData
    {
        std::vector<RollingBuffer*> rdata;
        bool   plotFlag = true;
        float* history;

        PlotData(void) = delete;
        PlotData(std::vector<RollingBuffer*> _rdata); // Deliberately copy whole vector
        ~PlotData(void) { delete history; } // rdata is cleared by the basicData object

        void SetDataToPlot(const std::vector<RollingBuffer*>& data) { rdata = data; }
        void PlotGraph(const std::vector<ColorPalette*>& plotColors);
    };
}

#endif