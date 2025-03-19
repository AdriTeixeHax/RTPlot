/// Plotter.h - Class responsible for managing the plot data and plotting it in real-time.

#ifndef _PLOTTER__H_
#define _PLOTTER__H_

#include <PlotData.h>
#include <RTPlotVars.h>

namespace RTPlot
{
    class Plotter
    {
        std::vector<PlotData*> data;
        std::string            name;
        char                   tempName[RTPLOT_TEMP_NAME_LEN];
        bool                   plotFlag = true;
        bool                   killPlot = false;
        float                  history; 

	public:
        Plotter(void);
        Plotter(std::vector<PlotData*>* pData);
        Plotter(const Plotter& p);
        ~Plotter(void);
        Plotter& operator=(const Plotter& pldata);

        // Getters
		std::vector<PlotData*>* GetDataPtr   (void)       { return &data; }
        size_t                  GetDataSize  (void) const { return  data.size(); }
        const std::string&      GetName      (void) const { return  name; }
        std::string*            GetNamePtr   (void)       { return &name; }
		char*                   GetTempName  (void)       { return  tempName; }
        bool                    GetKillPlot  (void) const { return  killPlot; }
        bool*                   GetKillPtr   (void)       { return &killPlot; }
		float*                  GetHistoryPtr(void)       { return &history; }
        float                   GetHistory   (void) const { return  history; }
        
        // Setters
        void                    SetDataToPlot(const std::vector<double>& originalData);

        // Actions
        void                    PlotGraph    (void);
        void                    ColorPicker  (uint32_t id);
    
        // JSON managing
        JSON                    toJSON       (void);
        void                    fromJSON     (const JSON& j);
        static std::vector<Plotter> ArrayFromJSON(const JSON& j);
    };
}

#endif