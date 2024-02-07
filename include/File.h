//#ifndef _FILE__H_
//#define _FILE__H_
//
//#include <fstream>
//#include <iostream>
//#include <sstream>
//
//namespace RTPlot
//{
//	class File
//	{
//        std::ofstream hWrite;
//        std::ifstream hRead;
//        std::string   filePath;
//
//    public:
//        File(const char* _filePath) : filePath(_filePath) { }
//
//        // Actions
//        bool Write(std::string content)
//        {
//            hWrite.open(filePath);
//
//        }
//	};
//}
//
//using namespace std;
//
//int main()
//{
//    // Declare an ofstream object to write to a file
//    ofstream outFile;
//    // Open the file with the name "example.txt"
//    outFile.open("example.txt");
//    // Check if the file is opened successfully
//    if (outFile.is_open())
//    {
//        // Write some text to the file
//        outFile << "This is a line.\n";
//        outFile << "This is another line.\n";
//        // Close the file
//        outFile.close();
//    }
//    else
//    {
//        // Handle the error
//        cout << "Error opening file for writing.\n";
//    }
//
//    // Declare an ifstream object to read from a file
//    ifstream inFile;
//    // Open the file with the name "example.txt"
//    inFile.open("example.txt");
//    // Check if the file is opened successfully
//    if (inFile.is_open())
//    {
//        // Declare a string variable to store a line
//        string line;
//        // Read the file line by line using a loop
//        while (getline(inFile, line))
//        {
//            // Print the line to the console
//            cout << line << endl;
//        }
//        // Close the file
//        inFile.close();
//    }
//    else
//    {
//        // Handle the error
//        cout << "Error opening file for reading.\n";
//    }
//
//    return 0;
//}
//
//#endif