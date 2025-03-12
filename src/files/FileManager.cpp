#include <FileManager.h>

std::string RTPlot::FileManager::Read(const std::string& fileName)
{
    std::ifstream inFile(fileName);
    std::string result;

    if (inFile.is_open()) 
    {
        std::string line;
        while (std::getline(inFile, line))
        {
            result.append(line);
            result.append("\n");
        }
        inFile.close();
        return result;
    }
    else return "[FileManager]: Error reading file!";
}

std::string RTPlot::FileManager::ReadFormat(const std::string& fileName, const std::string& format)
{
    size_t pos = fileName.rfind("." + format);
    if (pos != std::string::npos && pos + format.length() + 1 == fileName.length())
        return this->Read(fileName);
    else
        return "[FileManager]: Error reading file! Wrong format";
}

std::string RTPlot::FileManager::ReadLine(const std::string& fileName, const std::string& format, uint32_t lineNumber)
{
    std::string text = ReadFormat(fileName, format);
    std::stringstream ss(text);
    std::string line;
    size_t currentLine = 0;

    // Read lines from the string stream
    while (std::getline(ss, line)) 
    {
        currentLine++;
        if (currentLine == lineNumber) return line; // Return the line if the current line number matches
    }

    return ""; // Return an empty string if the line number is out of bounds
}

std::string RTPlot::FileManager::ReadAndStripPrefix(const std::string& fileName, const std::string& format, uint32_t line, const std::string& prefix)
{
    std::string result = ReadLine(fileName, format, line);
    size_t pos; // Iterator
    while ((pos = result.find(prefix)) != std::string::npos)
        result.erase(pos, prefix.length());
    return result;
}

std::vector<std::string> RTPlot::FileManager::ReadAndSeparate(const std::string& fileName, const std::string& format, uint32_t line, const std::string& separator)
{
    std::string input = ReadLine(fileName, format, line);
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = input.find(separator, start)) != std::string::npos) {
        result.push_back(input.substr(start, end - start));
        start = end + separator.length();
    }

    result.push_back(input.substr(start));

    return result;
}

std::string RTPlot::FileManager::OpenFileDialog()
{
    WCHAR filename[1024] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0RTPlot configuration files (*.rtplot)\0*.rtplot\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = 1024;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"Select a file to open";

    if (GetOpenFileName(&ofn))
    {
        std::wstring result(filename);
        return std::string(result.begin(), result.end());
    }

    return "";
}

bool RTPlot::FileManager::Write(const std::string& fileName, const std::string& msg)
{
    std::ofstream outFile(fileName);

    if (outFile.is_open())
    {
        outFile << msg;
        outFile.close();
        //std::cout << "[FileManager]: Data written to " << fileName << " successfully." << std::endl;
        return 0;
    }
    else 
    {
        std::cerr << "[FileManager]: Error opening file for writing." << std::endl;
        return false;
    }
}

bool RTPlot::FileManager::Append(const std::string& fileName, const std::string& msg)
{
    std::ofstream outFile(fileName, std::ios::app); // Open file in append mode
    if (!outFile) {
        std::cerr << "[FileManager]: Error opening file!" << std::endl;
        return false;
    }
    outFile << msg;
    outFile.close();
    //std::cout << "[FileManager]: Text appended successfully!" << std::endl;
    return true;
}
