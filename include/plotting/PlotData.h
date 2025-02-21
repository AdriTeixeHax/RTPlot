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
        std::vector<RollingBuffer> rdata; // Needs to be a non pointer vector so that data can be copied
        std::string                name;
        char                       tempName[32];
        bool                       plotFlag = true;
        bool                       killPlot = false;
        float*                     history;

        PlotData(void) = delete;
        PlotData(std::vector<RollingBuffer> _rdata); // Deliberately copy whole vector
        ~PlotData(void) { delete history; }
        PlotData operator= (const PlotData& pldata)
        {
            PlotData result(pldata.rdata);
            result.name = pldata.name;
            strcpy_s(result.tempName, pldata.tempName);
            result.plotFlag = pldata.plotFlag;
            result.killPlot = pldata.killPlot;
            *result.history = *pldata.history;
            return result;
        }

        // Getters
        const std::string& GetName   (void) { return  name; }
        bool*              GetKillPtr(void) { return &killPlot; }

        // Setters
        void SetDataToPlot(std::vector<double> data); // Yes, copy

        // Actions
        void PlotGraph(const std::vector<ColorPalette*>& plotColors);
    };
}

#endif