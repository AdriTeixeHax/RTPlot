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

#ifdef __linux__

	#include <cstring>
	#include <cerrno>

	using errno_t = int;

	inline errno_t strcpy_s(char* dest, size_t dest_size, const char* src) 
	{
		if (!dest || !src)
			return EINVAL; // Invalid argument

		if (strlen(src) >= dest_size)
			return ERANGE; // Buffer too small

		strcpy(dest, src);
		return 0; // Success
	}

#endif

#endif