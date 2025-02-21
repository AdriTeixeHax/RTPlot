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
        Graphics* graphicsPtr;
        bool      plotExitFlag   = false;
        bool      exitFlag       = true;
        uint8_t   id             = 0;

    public:
        RealTimePlot(void) = delete;
        RealTimePlot(Graphics* graphicsPtr);
        ~RealTimePlot(void);

        // Getters
        bool GetPlotExitFlag(void) const { return exitFlag; }

        // Setters
        void SetID        (uint8_t _id) { id = _id; }
        void SetDataToPlot(const std::vector<double>& data);

        // Actions
        int8_t Plot (const std::string& name, bool* killFlag, char* command, bool* sendCommand, bool* addVariable, uint32_t* varToRemove, bool* removeVariable);
        int8_t PlotGraph(uint8_t id, bool* killPlotFlag);
        int8_t PlotVars(uint8_t i, const std::string& portName, const std::vector<std::string>& currentNames, char* command, bool* sendCommand);
	};
}

#endif