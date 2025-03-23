#ifndef _PLOTDATA__H_
#define _PLOTDATA__H_

#include <iostream>
#include <fstream>
#include <vector>

#include <nlohmann/json.hpp>
#include <ColorPalette.h>
#include <RollingBuffer.h>

#include <RTPlotFunctions.h>

namespace RTPlot
{
    struct PlotData
    {
        ColorPalette  plotColor;
        RollingBuffer plotData;
        std::string   dataName;
        char          tempDataName[32] = "";
        bool          plottable = false;

        // Canonical form
        PlotData(void) : plotColor(ImVec4(0,0,0,0), 0), plotData(), dataName("") {}
        PlotData(uint32_t colorID, const char* name);
        PlotData(const PlotData& pdata);
        PlotData& operator= (const PlotData& pdata);
        ~PlotData(void) { }

        // Setters
        void SetSpan(float span) { plotData.SetSpan(span); }

        // Printers
        void Display(void);

        // JSON management
        JSON toJSON(void);
        void fromJSON(const JSON& j);
        static std::vector<PlotData> ArrayFromJSON(const JSON& j);
    };
}

#endif