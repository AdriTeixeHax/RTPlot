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
        std::string name;
        char   tempName[32];
        bool   plotFlag = true;
        bool   killPlot = false;
        float* history;

        PlotData(void) = delete;
        PlotData(std::vector<RollingBuffer*> _rdata); // Deliberately copy whole vector
        ~PlotData(void) { delete history; } // rdata is cleared by the basicData object

        bool* GetKillPtr(void) { return &killPlot; }
        const std::string& GetName(void) { return name; }
        void SetDataToPlot(const std::vector<RollingBuffer*>& data) { rdata = data; }
        void PlotGraph(const std::vector<ColorPalette*>& plotColors);
    };
}

#endif