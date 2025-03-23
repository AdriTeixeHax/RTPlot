#include <PlotData.h>

RTPlot::PlotData::PlotData(uint32_t colorID, const char* name) :
    plotColor(ColorPalette((ImVec4)ImColor::HSV(colorID / 7.0f, 1.0f, 1.0f), colorID)),
    plotData()
{
    dataName = name;
    strcpy_s(tempDataName, sizeof("New variable name"), "New variable name");
}

RTPlot::PlotData::PlotData(const PlotData& pdata)
{
    *this = pdata;
}

RTPlot::PlotData& RTPlot::PlotData::operator=(const PlotData& pdata)
{
    if (this != &pdata)
    {
        plotColor = pdata.plotColor;
        plotData = pdata.plotData;
        dataName = pdata.dataName;
        strcpy_s(tempDataName, sizeof(pdata.tempDataName), pdata.tempDataName);
        plottable = pdata.plottable;
    }
    return *this;
}

void RTPlot::PlotData::Display(void)
{
    std::cout << "Plot color:" << std::endl;
    std::cout << "x: " << plotColor.GetColor().x << std::endl;
    std::cout << "y: " << plotColor.GetColor().y << std::endl;
    std::cout << "z: " << plotColor.GetColor().z << std::endl;
    std::cout << "w: " << plotColor.GetColor().w << std::endl;

    std::cout << "Plot data:" << std::endl;
    std::cout << "span: " << plotData.GetSpan() << std::endl;
    std::cout << "plotFlag: " << plotData.GetPlotFlag() << std::endl;
}

JSON RTPlot::PlotData::toJSON(void)
{
    return JSON
    {
        {"dataName",  dataName},
        {"plottable", plottable},
        {"plotColor", plotColor.toJSON()},
        {"plotData",  plotData.toJSON()}
    };
}

void RTPlot::PlotData::fromJSON(const JSON& j)
{
    j.at("dataName").get_to(dataName);
    j.at("plottable").get_to(plottable);
    plotColor.fromJSON(j.at("plotColor"));
    plotData.fromJSON(j.at("plotData"));
}

std::vector<RTPlot::PlotData> RTPlot::PlotData::ArrayFromJSON(const JSON& j)
{
    std::vector<PlotData> plotterVec;

    for (const auto& jIter : j)
    {
        PlotData p;
        p.fromJSON(jIter);
        plotterVec.push_back(p);
    }
    return plotterVec;
}
