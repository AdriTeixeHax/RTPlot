/// Plotter.h - Class responsible for managing the plot data and plotting it in real-time.

#ifndef _PLOTTER__H_
#define _PLOTTER__H_

#include <vector>

#include <plotting/ScrollingBuffer.h>
#include <plotting/RollingBuffer.h>
#include <plotting/ColorPalette.h>

namespace RTPlot
{
    struct PlotData
    {
        ColorPalette*  plotColor;
        RollingBuffer* plotData;
        std::string    dataName;
        char           tempDataName[32];
        bool           plottable = false;

        PlotData(void) = delete;
        PlotData(uint32_t colorID, const char* name) :
            plotColor(new ColorPalette((ImVec4)ImColor::HSV(colorID / 7.0f, 1.0f, 1.0f), colorID)),
            plotData(new RollingBuffer)
        {
			dataName = name;
			strcpy_s(tempDataName, "New variable name");
        }
		PlotData& operator= (const PlotData& pdata)
		{
			if (this != &pdata)
			{
				*plotColor = *pdata.plotColor;
				*plotData  = *pdata.plotData;
				 dataName  =  pdata.dataName;
				strcpy_s(tempDataName, pdata.tempDataName);
			}
			return *this;
		}
		~PlotData(void) { delete plotColor; delete plotData; }

        // Setters
        void SetSpan(float span) { plotData->SetSpan(span); }
    };

    class Plotter
    {
        std::vector<PlotData*> data;
        std::string            name;
        char                   tempName[32];
        bool                   plotFlag = true;
        bool                   killPlot = false;
        float*                 history; 

    private:
        Plotter(void);

	public:
        Plotter(std::vector<PlotData*>* pData);
        ~Plotter(void);
        Plotter& operator=(const Plotter& pldata);

        // Getters
		std::vector<PlotData*>* GetDataPtr   (void)       { return &data; }
        size_t                  GetDataSize  (void) const { return  data.size(); }
        const std::string&      GetName      (void) const { return  name; }
        std::string*            GetNamePtr   (void)       { return &name; }
		char*                   GetTempName  (void)       { return  tempName; }
        bool*                   GetKillPtr   (void)       { return &killPlot; }
		float*                  GetHistoryPtr(void)       { return  history; }
        float                   GetHistory   (void) const { return *history; }
        
        // Setters
        void                    SetDataToPlot(const std::vector<double>& originalData);

        // Actions
        void                    PlotGraph(void);
        void                    ColorPicker(uint32_t id) { if (data.size() > id) data.at(id)->plotColor->ColorPicker(); }
    };
}

#endif