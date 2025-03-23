/// FileManager.h - Class to manage files, responsible of reading and writing to them.

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#ifdef _WIN32
	#include <Windows.h>
	#include <commdlg.h>
	#pragma comment(lib, "Comdlg32.lib")
#endif
#ifdef __linux__
    #include <fcntl.h>
	#include <array>
	#include <memory>
#endif

#include <RTPlotVars.h>

namespace RTPlot
{
	class FileManager
	{
	private:
		std::string              Read                  (const std::string& fileName);

	public:
		std::string              ReadFormat            (const std::string& fileName, const std::string& format);
		std::string              ReadLine              (const std::string& text, uint32_t line);
		std::string              ReadAndStripPrefix    (const std::string& fileName, const std::string& format, uint32_t line, const std::string& prefix);
		
		std::string				 OpenFileDialog        (void);
		std::string				 SaveFileDialog		   (void);

		bool                     Write                 (const std::string& fileName, const std::string& msg);
		bool                     Append                (const std::string& fileName, const std::string& msg);
		
		static std::vector<std::string> ReadAndSeparate(const std::string& text, const std::string& separator);
	};
}