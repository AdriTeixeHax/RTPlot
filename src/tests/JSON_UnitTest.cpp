#include <iostream>
#include <fstream>
#include <vector>

#include <SerialPlotter.h>

using namespace RTPlot;

void saveToFile(SerialPlotter& data, const std::string& filename)
{
    JSON j = data.toJSON();

    std::ofstream file(filename);
    if (file)
    {
        file << j.dump(4); // Pretty print JSON with 4-space indentation
        std::cout << "Data saved to " << filename << "\n";
    }
    else
    {
        std::cout << "Failed to open file for writing!\n";
    }
}

// Function to load a vector of Persons from a JSON file
SerialPlotter loadFromFile(const std::string& filename, std::string* logMsgPtr)
{
    SerialPlotter data("\\\\.\\COM6", logMsgPtr);
    std::ifstream file(filename);
    if (file)
    {
        JSON j;
        file >> j;
        data.fromJSON(j);

        std::cout << "Data loaded from " << filename << "\n";
    }
    else
    {
        std::cout << "Failed to open file for reading!\n";
    }
    return data;
}

int main()
{
    std::string logMsg;
    SerialPlotter* serialPlotter = new SerialPlotter("COM6", &logMsg);

    serialPlotter->GetSerialDevice()->SetVerbose(false);
    serialPlotter->GetSerialDevice()->SetPortVerbose(false);

    serialPlotter->GetRealTimePlotter()->GetPlotters()->push_back(new Plotter);

    serialPlotter->GetSerialDevice()->GetPort()->SetTimeouts(100, 90, 80, 1, 26);

    // Save to JSON file
    saveToFile(*serialPlotter, "bin/example.json");

    delete serialPlotter;
        
    // Load from JSON file
    SerialPlotter* loadedData = new SerialPlotter(loadFromFile("bin/example.json", &logMsg));

    delete loadedData;

    return 0;
}