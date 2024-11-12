/* main.cpp | Main file for RTPlot by AdriTeixeHax */

// IO
#include <iostream>
#include <sstream>
#include <cctype>

// Callbacks and threads
#include <thread>
#include <mutex>

// Graphics
#include "Shader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"

#include <plotting/Logger.h>


//// OpenGL math
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>

// Serial devices
#include "DeviceManager.h"

// Tests
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

// Graphics struct
#include "Graphics.h"

/**********************************************************************************************/

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

        static bool verboseFlag = false;
        static bool ImGuiDemoFlag = false;
        static bool ImPlotDemoFlag = false;
        static bool consoleLogFlag = false;
        static bool showAddPlotFlag = true;
        static bool serialOptionsFlag = false;
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
                for (uint8_t i = 0; i < deviceManager.size(); i++)
                    deviceManager[i]->serialDevice->getPort()->setTimeouts(wtm, rtm, ri, rtc, wtc);
                logMsg = "Applied serial parameters.\n";
                serialOptionsFlag = false;
            }

            ImGui::End();
        }

        // Serial plotting window
        ImGui::Begin("RTPlot - by AdriTeixeHax", NULL); // Null so that it cannot be closed
        if (ImPlotDemoFlag) ImPlot::ShowDemoWindow(&ImPlotDemoFlag);
        if (ImGuiDemoFlag)   ImGui::ShowDemoWindow(&ImGuiDemoFlag);

        static std::vector<uint8_t> serialPorts;
        if (ImGui::Button("Add Device"))
        {
            serialPorts = RTPlot::SerialPort::scanAvailablePorts();
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

                std::string port = "COM" + std::to_string(device);
                if (ImGui::Button(port.c_str()))
                {
                    const char* portName = port.c_str();
                    deviceManager.AddDevice(portName);
                    showAddPlotFlag = false;
                    logMsg = "Added device " + port + "\n";
                }

                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
        }
        
        static std::vector<std::string> portsOpen;
        if (ImGui::Button("Remove Device"))
        {
            portsOpen.clear();
            for (uint8_t i = 0; i < deviceManager.size(); i++)
            {
                const char* portName = deviceManager[i]->serialDevice->getPort()->getName().c_str();
                portsOpen.push_back(portName);
            }
            showDeletePlotFlag = true;
        }

        if (!consoleLogFlag)
        {
            if (ImGui::Button("Console Log"))
            {
                consoleLogFlag = true;
            }
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

        // Plotting
        deviceManager.plotAll();

        if (consoleLogFlag) RTPlot::ShowConsoleLog(logMsg, &consoleLogFlag);

        ImGui::End();

        // GUI rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        glfwSwapBuffers(graphics->window); // Swap front and back buffers
        glfwPollEvents(); // Poll and process events

    } // !while

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