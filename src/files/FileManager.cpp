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

std::string RTPlot::FileManager::ReadLine(const std::string& text, uint32_t lineNumber)
{
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
    std::string result = ReadLine(ReadFormat(fileName, format), line);
    size_t pos; // Iterator
    while ((pos = result.find(prefix)) != std::string::npos)
        result.erase(pos, prefix.length());
    return result;
}

std::vector<std::string> RTPlot::FileManager::ReadAndSeparate(const std::string& text, const std::string& separator)
{
    std::vector<std::string> result;
    size_t start = 0;
    size_t end;

    while ((end = text.find(separator, start)) != std::string::npos) {
        result.push_back(text.substr(start, end - start));
        start = end + separator.length();
    }

    result.push_back(text.substr(start));

    return result;
}

#ifdef _WIN32

std::string RTPlot::FileManager::OpenFileDialog(void)
{
    WCHAR filename[1024] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0JSON configuration files (*.json)\0*.json\0";
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

std::string RTPlot::FileManager::SaveFileDialog(void)
{
    WCHAR filename[1024] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFilter = L"All Files (*.*)\0*.*\0JSON configuration files (*.json)\0*.json\0";
    ofn.lpstrFile = filename;
    ofn.nMaxFile = 1024;
    ofn.Flags = OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST;
    ofn.lpstrTitle = L"Select a file to save";

    if (GetSaveFileName(&ofn)) // <-- Changed to GetSaveFileName()
    {
        std::wstring result(filename);
        return std::string(result.begin(), result.end());
    }

    return "";
}

#endif

#ifdef __linux__

std::string RTPlot::FileManager::OpenFileDialog(void)
{
    std::array<char, 1024> buffer;
    std::string result;

    // Open a file dialog using Zenity (native Linux GUI)
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("zenity --file-selection --title='Select a file to open' 2>/dev/null", "r"), pclose);

    if (!pipe) {
        std::cerr << "[FileManager]: Failed to open file dialog.\n";
        return "";
    }

    // Read the file path returned by Zenity
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    // Remove trailing newline (Zenity outputs the filename with a newline)
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

std::string RTPlot::FileManager::SaveFileDialog(void)
{
    std::array<char, 1024> buffer;
    std::string result;

    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("zenity --file-selection --save --confirm-overwrite --title='Save file as' 2>/dev/null", "r"), pclose);

    if (!pipe) {
        std::cerr << "[FileManager]: Failed to open save dialog.\n";
        return "";
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

#endif

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
