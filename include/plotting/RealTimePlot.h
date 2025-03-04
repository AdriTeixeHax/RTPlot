/// RealTimePlot.h - Class to manage printing in real-time and other GUI functions.

#ifndef _REALTIMEPLOT__H_
#define _REALTIMEPLOT__H_

#include <cmath>
#include <cstdint>
#include <string>

#include <plotting/PlotData.h>

namespace RTPlot
{
	class RealTimePlot
	{
        std::vector<ColorPalette*> plotColors;
        std::vector<PlotData*>     plotData;
        std::vector<RollingBuffer> basicData;
		uint8_t   dataNum = 1;                  // Number of variables to plot. First one is always time.
		Graphics* graphicsPtr;                  // Pointer to the graphics object, needed to access some functions.
		bool      exitFlag = true;              // If false, the object doesn't plot anymore.

    public:
        RealTimePlot(void) = delete;
        RealTimePlot(Graphics* graphicsPtr);
        ~RealTimePlot(void);

        // Getters
        bool GetPlotExitFlag(void) const { return exitFlag; }

        // Setters
        void SetDataToPlot(const std::vector<double>& data);

        // Actions
        int8_t Plot (const std::string& name, bool* killFlag, char* command, bool* sendCommand, bool* addVariable, uint32_t* varToRemove, bool* removeVariable);
        int8_t PlotGraph(uint8_t id, bool* killPlotFlag);
        int8_t PlotVars(uint8_t i, const std::string& portName, const std::vector<std::string>& currentNames, char* command, bool* sendCommand);
	};
}

#endif