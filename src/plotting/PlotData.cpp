#include <plotting/PlotData.h>

namespace RTPlot
{
    PlotData::PlotData(std::vector<RollingBuffer*> _rdata) : // Deliberately copy whole vector
        history(new float(10.0f)) 
    {
        rdata = _rdata;
    }

    void PlotData::PlotGraph(const std::vector<ColorPalette*>& plotColors)
    {
        for (uint32_t i = 0; i < rdata.size(); i++)
        {
            if (plotFlag && rdata.at(i)->data.Size > 0)
            {
                ImPlot::PushStyleColor(ImPlotCol_Line, plotColors.at(i)->GetColor());
                ImPlot::PlotLine(rdata.at(i)->name.c_str(), &rdata.at(i)->data[0].x, &rdata.at(i)->data[0].y, rdata.at(i)->data.size(), 0, 0, 2 * sizeof(float));
                ImPlot::PopStyleColor();
            }
        }
    }
}