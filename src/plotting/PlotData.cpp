#include <plotting/PlotData.h>

namespace RTPlot
{
    PlotData::PlotData(std::vector<RollingBuffer> _rdata) : // Deliberately copy whole vector
        history(new float(10.0f))
    {
        rdata = _rdata;
        name = "New Plot";
        strcpy_s(tempName, "New plot name");
    }

    void PlotData::SetDataToPlot(std::vector<double> data) // Yes, copy
    {
        if (data.size() <= 0) return;

        for (size_t i = 0; i < rdata.size(); i++)
        {
            if (i < data.size() - 1)
                rdata.at(i).AddPoint(data[0], data[i + 1]);
        }
    }

    void PlotData::PlotGraph(const std::vector<ColorPalette*>& plotColors)
    {
        for (uint32_t i = 0; i < rdata.size(); i++)
        {
            if (plotFlag && rdata.at(i).plotFlag && rdata.at(i).data.Size > 0)
            {
                ImPlot::PushStyleColor(ImPlotCol_Line, plotColors.at(i)->GetColor());
                ImPlot::PlotLine(
                     rdata.at(i).name.c_str(), 
                    &rdata.at(i).data[0].x,
                    &rdata.at(i).data[0].y,
                     rdata.at(i).data.Size, 
                    0, 0, 2 * sizeof(float)
                );
                ImPlot::PopStyleColor();
            }
        }
    }
}