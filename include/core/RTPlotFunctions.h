/// RTPlotFunctions.h - Inline functions used globaly.

#ifndef _RTPLOTFUNCTIONS__H_
#define _RTPLOTFUNCTIONS__H_

#include <string>

constexpr const char* LONG_COM_PORT_PREFIX = "\\\\.\\";

namespace RTPlot
{
	void Toggle(bool& var);

	std::string StripPortNamePrefix(const std::string& name);
}

#endif