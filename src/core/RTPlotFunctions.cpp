#include <RTPlotFunctions.h>
#include <cstdint>

void RTPlot::Toggle(bool& var)
{ 
	var = !var; 
}

std::string RTPlot::StripPortNamePrefix(const std::string& name)
{
	std::string result = name;
	size_t pos; // Iterator
	while ((pos = result.find(LONG_COM_PORT_PREFIX)) != std::string::npos)
		result.erase(pos, strlen(LONG_COM_PORT_PREFIX));
	return result;
}