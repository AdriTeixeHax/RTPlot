#include <iostream>
#include <fstream>
#include <string>

#include <FileManager.h>

void FileManager_UnitTest(void)
{
    RTPlot::FileManager fileManager;

    std::string msg = "Test message\n";
    std::string read;
    std::vector<std::string> readVector;

    std::string filename = fileManager.OpenFileDialog();

    fileManager.Write(filename, msg);

    for (auto i = 0; i < 6; i++)
    {
        fileManager.Append(filename, "asdf:" + std::to_string(i) + "\n");
    }

    read = fileManager.ReadFormat(filename, RTPLOT_FORMAT);
    std::cout << "Read whole text with its format: " << read << std::endl;

    read = fileManager.ReadLine(filename, RTPLOT_FORMAT, 3);
    std::cout << "Read line 3: " << read << std::endl;
    
    read = fileManager.ReadAndStripPrefix(filename, RTPLOT_FORMAT, 3, "asdf:");
    std::cout << "Read and strip prefix: " << read << std::endl;

    readVector = fileManager.ReadAndSeparate(filename, RTPLOT_FORMAT, 3, ":");
    std::cout << "Read and separate: ";
    for (auto i : readVector) std::cout << i << " ";
}

//#define FILEMANAGER_UNITTEST

#ifdef FILEMANAGER_UNITTEST
int main()
{
    FileManager_UnitTest();
    return 0;
}
#endif