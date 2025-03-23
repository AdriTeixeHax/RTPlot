#include <Plotter.h>

#include <implot/implot.h>

#include <RTPlotVars.h>

#include <iostream>

namespace RTPlot
{
    Plotter::Plotter(void) : 
        history(10.0f)
    {
        name = "New Plot";
        strcpy_s(tempName, sizeof("New plot name"), "New plot name");

		for (size_t i = 0; i < RTPLOT_DATA_SIZE - 1; i++)
		{
			data.push_back(new PlotData(i, std::to_string(i).c_str()));
		}
    }
    
    Plotter::Plotter(std::vector<PlotData*>* pData) :
		history(10.0f)
    {
        name = "New Plot";
        strcpy_s(tempName, sizeof("New plot name"), "New plot name");

        for (size_t i = 0; i < pData->size(); i++)
        {
            data.push_back(new PlotData(i, std::to_string(i).c_str()));
        }

        for (size_t i = 0; i < data.size(); i++)
        {
            data.at(i)->plotData  = pData->at(i)->plotData;
            data.at(i)->plotColor = pData->at(i)->plotColor;
            data.at(i)->plottable = pData->at(i)->plottable;
        }
    }

    Plotter::Plotter(const Plotter& p) 
    {
        *this = p;
    }

    Plotter::~Plotter(void)
    {
        for (auto i : data) delete i;
    }

    Plotter& Plotter::operator=(const Plotter& pldata)
    {
        if (this != &pldata)
        {
            this->data.clear();
            for (uint32_t i = 0; i < pldata.GetDataSize(); i++)
			{
                this->data.push_back(new PlotData(i, pldata.name.c_str()));
                *(this->data.at(i)) = *(pldata.data.at(i));
			}

            // Copy simple data types
            this->name = pldata.name;
            strcpy_s(this->tempName, sizeof(this->tempName), pldata.tempName);
            this->plotFlag = pldata.plotFlag;
            this->killPlot = pldata.killPlot;
            this->history = pldata.history;
        }

        return *this;
    }

    void Plotter::SetDataToPlot(const std::vector<double>& originalData)
    {
        if (originalData.size() < 2) { std::cerr << "[Plotter]: Error setting data to plot." << std::endl; return; }

		for (size_t i = 0; i < data.size() - 1; i++)
            data.at(i)->plotData.AddPoint(originalData[0], originalData[i + 1]);
    }

    void Plotter::PlotGraph(void)
    {
        for (uint32_t i = 0; i < data.size(); i++)
        {
            RollingBuffer rdata = data.at(i)->plotData;
            ColorPalette  color = data.at(i)->plotColor;
            const char*   name  = data.at(i)->dataName.c_str();
            size_t        size  = rdata.GetDataRef().size();

            if (size <= 0) return;

            if (this->plotFlag && rdata.GetPlotFlag() && data.at(i)->plottable)
            {
                ImPlot::PushStyleColor(ImPlotCol_Line, color.GetColor());
                    ImPlot::PlotLine(name, &rdata.GetDataRef()[0].x, &rdata.GetDataRef()[0].y, size, 0, 0, 2 * sizeof(float));
                ImPlot::PopStyleColor();
            }
        }
    }

    void Plotter::ColorPicker(uint32_t id)
    {
        data.at(id)->plotColor.ColorPicker();
    }

    JSON Plotter::toJSON(void)
    {
        JSON j;
        JSON dataArray = JSON::array();

        for (const auto& i : data) 
            if (i) dataArray.push_back(i->toJSON());

        j["data"]     = dataArray;
        j["name"]     = name;
        j["plotFlag"] = plotFlag;
        j["history"]  = history;

        return j;
    }


    void Plotter::fromJSON(const JSON& j)
    {
        j.at("name").get_to(name);
        j.at("plotFlag").get_to(plotFlag);
        j.at("history").get_to(history);

        std::vector<PlotData> dataVec = PlotData::ArrayFromJSON(j.at("data"));
        for (auto i : data) delete i;
        data.clear();

        for (size_t i = 0; i < dataVec.size(); i++)
            data.push_back(new PlotData(dataVec.at(i)));
    }

    std::vector<Plotter> Plotter::ArrayFromJSON(const JSON& j)
    {
        std::vector<Plotter> plotterVec;

        for (const auto& jIter : j)
        {
            Plotter p;
            p.fromJSON(jIter);
            plotterVec.push_back(p);
        }
        return plotterVec;
    }
}