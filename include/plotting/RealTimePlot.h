/// RealTimePlot.h - Class to manage printing in real-time and other GUI functions.

#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include <plotting/Plotter.h>

namespace RTPlot
{
	class RealTimePlot
	{
		std::vector<Plotter*>   plotters;           // Array of plotter objects. No. of plotters = no. of graphs.
		std::vector<PlotData*>  basicData;          // Basic array to store a copy of the obtained values from the serial port.
		bool                    exitFlag = true;    // If false, the object doesn't plot anymore.
        size_t                  visibleVarsNum = 0; // Number of visible variables
        bool                    serialOptionsFlag = true;

    public:
        RealTimePlot(void);
        ~RealTimePlot(void);

        // Getters
        bool                    GetPlotExitFlag        (void) const { return exitFlag; }
        bool*                   GetSerialOptionsFlagPtr(void)       { return &serialOptionsFlag; }
        std::vector<Plotter*>*  GetPlotters            (void)       { return &plotters; }
        std::vector<PlotData*>* GetBasicData           (void)       { return &basicData; }

        // Setters
        void                    SetDataToPlot          (const std::vector<double>& data);
        void                    SetVisibleVarsNum      (size_t num) { visibleVarsNum = num; }
        void                    SetSerialOptionsFlag   (bool so) { serialOptionsFlag = so; }
        
        // Actions
        int8_t                  Plot                   (const std::string& name, const std::string& friendlyName, bool* killFlag, char* command, bool* sendCommand, bool* addVariable, uint32_t* varToRemove, bool* removeVariable);
        int8_t                  PlotGraph              (uint8_t id, bool* killPlotFlag);
        int8_t                  PlotVars               (uint8_t i, const std::string& portName, const std::vector<std::string>& currentNames, char* command, bool* sendCommand);
	
        // JSON managing
        JSON                    toJSON                 (void);
        void                    fromJSON               (const JSON& j);
    };
}

#endif