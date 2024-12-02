/* main.cpp | Main file for RTPlot by AdriTeixeHax */

// IO
#include <iostream>
#include <sstream>
#include <cctype>

#include <plotting/Logger.h>

// Serial devices
#include <serial/DeviceManager.h>

/*********************************************************/

// Main function
int main(int argc, char** argv)
{
    // Create graphics instance
    RTPlot::Graphics* graphics = new RTPlot::Graphics;
    
    // Initialize graphics, GUI and load the logo
    if (!graphics->GraphicsInit()) return -1;
    if (!graphics->GuiInit())      return -1;
    if (!graphics->LoadLogo())     return -1;

    //// Create a rendering instance for custom models
    //Renderer renderer;

    // Create a device manager instance
    RTPlot::DeviceManager deviceManager;

    /******************** MAIN LOOP ********************/

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(graphics->window))
    {
        graphics->NewFrame();

        static bool verboseFlag        = true;
        static bool ImGuiDemoFlag      = false;
        static bool ImPlotDemoFlag     = false;
        static bool consoleLogFlag     = true;
        static bool showAddPlotFlag    = true;
        static bool serialOptionsFlag  = false;
        static bool showDeletePlotFlag = true;

        std::string logMsg;

        // Menu Bar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Options"))
            {
                if (ImGui::MenuItem("ImPlot Demo", "", ImPlotDemoFlag)) { ImPlotDemoFlag = !ImPlotDemoFlag; }
                if (ImGui::MenuItem("ImGui Demo",  "", ImGuiDemoFlag))  { ImGuiDemoFlag  = !ImGuiDemoFlag; }
                ImGui::Separator();
                if (ImGui::MenuItem("Verbose data", "", verboseFlag))
                {
                    verboseFlag = !verboseFlag;
                    if (verboseFlag) logMsg = "Turned on verbose.\n";
                    else             logMsg = "Turned off verbose.\n";
                }
                if (ImGui::MenuItem("Console log",    "", consoleLogFlag))    { consoleLogFlag = !consoleLogFlag; }
                if (ImGui::MenuItem("Serial options", "", serialOptionsFlag)) { serialOptionsFlag = !serialOptionsFlag; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
        ImGui::End();

        // Serial Options tab
        if (serialOptionsFlag)
        {
            ImGui::Begin("Serial Options", &serialOptionsFlag);
            static int wtm = 10, rtm = 10, ri = 50, rtc = 1000, wtc = 1000; // Serial parameters

            ImGui::InputInt("Write Total Multiplier", &wtm);
            ImGui::InputInt("Write Total Constant",   &wtc);
            ImGui::InputInt("Read Total Multiplier",  &rtm);
            ImGui::InputInt("Read Total Constant",    &rtc);
            ImGui::InputInt("Read Interval",          &ri);

            if (ImGui::Button("Apply"))
            {
                for (uint8_t i = 0; i < deviceManager.Size(); i++)
                    deviceManager[i]->serialDevice->GetPort()->SetTimeouts(wtm, rtm, ri, rtc, wtc);
                logMsg = "Applied serial parameters.\n";
            }

            ImGui::End();
        }

        // Main window - Serial Plotting occurs here
        ImGui::Begin("RTPlot - by AdriTeixeHax", NULL); // Null so that it cannot be closed
            if (ImPlotDemoFlag) ImPlot::ShowDemoWindow(&ImPlotDemoFlag);
            if (ImGuiDemoFlag)   ImGui::ShowDemoWindow(&ImGuiDemoFlag);

            static std::vector<uint8_t> serialPorts;
            if (ImGui::Button("List Devices"))
            {
                serialPorts = RTPlot::SerialPort::ScanAvailablePorts();
                showAddPlotFlag = true;
            }

            if (showAddPlotFlag)
            {
                for (uint8_t device : serialPorts)
                {
                    ImGui::SameLine();

                    ImGui::PushID(device);
                    ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(device / 7.0f, 1.0f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(device / 7.0f, 0.7f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(device / 7.0f, 0.8f, 0.8f));

                    std::string portName = "\\\\.\\COM" + std::to_string(device);
                    std::string portNameGUI = "COM" + std::to_string(device);
                    if (ImGui::Button(portNameGUI.c_str()))
                    {
                        deviceManager.AddDevice(portName.c_str(), graphics);
                        showAddPlotFlag = false;
                        logMsg = "Added device " + portNameGUI + "\n";
                    }

                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                }
            }
        
            static std::vector<std::string> portsOpen;
            if (ImGui::Button("Remove Device"))
            {
                portsOpen.clear();
                for (uint8_t i = 0; i < deviceManager.Size(); i++)
                {
                    const char* portName = deviceManager[i]->serialDevice->GetPort()->GetName().c_str();
                    portsOpen.push_back(portName);
                }
                showDeletePlotFlag = true;
            }

            if (showDeletePlotFlag)
            {
                for (uint8_t i = 0; i < portsOpen.size(); i++)
                {
                    ImGui::SameLine();

                    ImGui::PushID(i);
                    ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(i / 7.0f, 1.0f, 0.6f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(i / 7.0f, 0.7f, 0.7f));
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(i / 7.0f, 0.8f, 0.8f));

                    if (ImGui::Button(portsOpen[i].c_str()))
                    {
                        deviceManager.RemoveDevice(i);
                        portsOpen.erase(portsOpen.begin() + i);
                        showDeletePlotFlag = false;
                        logMsg = "Deleted device\n";
                    }

                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                }
            }

            if (!consoleLogFlag)
            {
                if (ImGui::Button("Console Log"))
                {
                    consoleLogFlag = true;
                }
            }

            // Plotting
            deviceManager.PlotDevices();

            // Plot the log message of the current cycle
            if (consoleLogFlag) ImGui::Log::ShowConsoleLog(logMsg, &consoleLogFlag);
        ImGui::End();

        graphics->GuiEnd();
        graphics->EndFrame();

    } // !while

    deviceManager.~DeviceManager(); // To end threads and mutex present in its components.

    // GUI shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // GLFW shutdown
    glfwTerminate();

    // Delete graphics instance
    delete graphics;

    return 0;
}