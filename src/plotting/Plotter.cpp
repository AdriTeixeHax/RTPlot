#include <plotting/Plotter.h>

#include <../src/vendor/implot/implot.h>

#include <core/RTPlotVars.h>

#include <iostream>

namespace RTPlot
{
    Plotter::Plotter(void) : 
        history(new float(10.0f))
    {
        name = "New Plot";
        strcpy_s(tempName, "New plot name");

		for (uint8_t i = 0; i < RTPLOT_DATA_SIZE - 1; i++)
		{
			data.push_back(new PlotData(i, std::to_string(i).c_str()));
		}
    }
    
    Plotter::Plotter(std::vector<PlotData*>* pData) :
		history(new float(10.0f))
    {
        name = "New Plot";
        strcpy_s(tempName, "New plot name");

        for (uint8_t i = 0; i < pData->size(); i++)
        {
            data.push_back(new PlotData(i, std::to_string(i).c_str()));
        }

        for (size_t i = 0; i < data.size(); i++)
        {
            *(data.at(i)->plotData)  = *(pData->at(i)->plotData);
            *(data.at(i)->plotColor) = *(pData->at(i)->plotColor);
              data.at(i)->plottable  =   pData->at(i)->plottable;
        }
    }

    Plotter::~Plotter(void)
    {
		if (history) delete history;
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

            // Handle history deep copy
            if (this->history) delete[] this->history; // Free old memory
            if (pldata.history)
            {
                this->history = new float; // Allocate new memory
                *this->history = *pldata.history; // Copy contents
            }
            else this->history = nullptr; // Nullify if source is null
        }

        return *this;
    }

    void Plotter::SetDataToPlot(const std::vector<double>& originalData)
    {
        if (originalData.size() < 2) { std::cerr << "[Plotter]: Error setting data to plot." << std::endl; return; }

		for (size_t i = 0; i < data.size() - 1; i++)
            data.at(i)->plotData->AddPoint(originalData[0], originalData[i + 1]);
    }

    void Plotter::PlotGraph(void)
    {
        for (uint32_t i = 0; i < data.size(); i++)
        {
            RollingBuffer*   rdata = data.at(i)->plotData;
            ColorPalette*    color = data.at(i)->plotColor;
            const char*      name  = data.at(i)->dataName.c_str();
            size_t           size  = rdata->GetDataRef().size();

            if (size <= 0) return;

            if (this->plotFlag && rdata->GetPlotFlag() && data.at(i)->plottable)
            {
                ImPlot::PushStyleColor(ImPlotCol_Line, color->GetColor());
                ImPlot::PlotLine(name, &rdata->GetDataRef()[0].x, &rdata->GetDataRef()[0].y, size, 0, 0, 2 * sizeof(float));
                ImPlot::PopStyleColor();
            }
        }
    }
}