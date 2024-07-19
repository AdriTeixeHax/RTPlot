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
#include <stb_image/stb_image.h>

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Plotting and UI
#include <implot/implot.h>
#include <implot/implot_internal.h>
#include <plotting/RealTimePlot.h>
#include <plotting/Logger.h>

// Serial devices
#include "DeviceManager.h"

// Tests
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

// Window name
#define RTPLOT_WINDOW_NAME "RTPlot - by AdriTeixeHax"

// Logo path
#define RTPLOT_LOGO_PATH "res/logos/RTPlot.png"

// Font paths
#define RTPLOT_SOURCESANS_PATH "res/fonts/SourceSans/SourceSans3-Regular.ttf"
#define RTPLOT_SOURCESANS_SIZE 20
#define RTPLOT_CONSOLA_PATH "res/fonts/Consola/Consola.ttf"
#define RTPLOT_CONSOLA_SIZE 15

// UI Parameters
#define RTPLOT_WINDOW_RADIUS 5
#define RTPLOT_MAIN_RADIUS 3

/**********************************************************************************************/

// Main function
int main(int argc, char** argv)
{
    /******************** GLFW - GLEW CONFIG ********************/

    // Create window context and get data from main monitor
    GLFWwindow* window;

    // Initialize GLFW
    if (!glfwInit()) { std::cerr << "[GLFW]: Error initializing GLFW." << std::endl; return -1; }
    // Set OpenGL version: 4.6 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1920, 1080, RTPLOT_WINDOW_NAME, NULL, NULL);
    if (!window) { std::cerr << "[GLFW]: Error creating window." << std::endl; glfwTerminate(); return -1; }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) { std::cerr << "[GLEW]: Error initializing GLEW." << std::endl; return -1; }

    // Print out the OpenGL version
    std::cout << "[OPENGL]: OpenGL version: " << glGetString(GL_VERSION) << std::endl << std::endl;

    // Enable and set blending
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    /******************** IMGUI - IMPLOT CONFIG ********************/

    // ImGui and ImPlot context creation
    ImGui::CreateContext();
    ImPlot::CreateContext();

    // ImGui IO configuration
    ImGuiIO& io = ImGui::GetIO(); (void)io;                                       // Get the IO configuration from ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;                         // Enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;                          // Enable gamepad controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;                             // Enable docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;                           // Enable viewports
    io.Fonts->AddFontFromFileTTF(RTPLOT_SOURCESANS_PATH, RTPLOT_SOURCESANS_SIZE); // Add the SourceSans font
    io.Fonts->AddFontFromFileTTF(RTPLOT_CONSOLA_PATH,    RTPLOT_CONSOLA_SIZE);    // Add the Consolas font
    // ImGui element radius setting
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,     RTPLOT_MAIN_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding,     RTPLOT_MAIN_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding,      RTPLOT_MAIN_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding,     RTPLOT_MAIN_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, RTPLOT_MAIN_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_TabRounding,       RTPLOT_MAIN_RADIUS);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,    RTPLOT_WINDOW_RADIUS);

    // Set the default color scheme to dark
    ImGui::StyleColorsDark();

    // Link the GLFW context with ImGui and initialize the OpenGL version for ImGui
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Create a rendering instance
    Renderer renderer;

    /******************** RTPLOT CONFIG ********************/

    // Initialize threads
    bool threadExitFlag = false;
    RTPlot::DeviceManager deviceManager;
    //std::thread readingThread(serialReadingFunc, &threadExitFlag, microController, &reading);

    // Disable ugly menu arrow
    ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;

    // Load app logo
    GLFWimage* appLogo = new GLFWimage();
    appLogo->pixels = stbi_load(RTPLOT_LOGO_PATH, &appLogo->width, &appLogo->height, nullptr, 4);
    glfwSetWindowIcon(window, 1, appLogo);
    delete appLogo;

    /******************** MAIN LOOP ********************/

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window))
    {
        // Clear color buffer bit
        glCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
        renderer.clear();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // UI Elements (view imgui_demo.cpp for comments regarding flag choices)
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;

        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) window_flags |= ImGuiWindowFlags_NoBackground;

        if (!opt_padding) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

        ImGui::Begin("RTPlot", NULL, window_flags);
        if (!opt_padding)   ImGui::PopStyleVar();
        if (opt_fullscreen) ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

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
                    logMsg = "Deleted device";
                }

                ImGui::PopStyleColor(3);
                ImGui::PopID();
            }
        }

        // Plotting
        deviceManager.plotAll();

        ImGui::End();

        if (consoleLogFlag) RTPlot::ShowConsoleLog(logMsg, &consoleLogFlag);

        // GUI rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        glfwSwapBuffers(window);// Swap front and back buffers
        glfwPollEvents(); // Poll and process events

    } // !while

    // GUI shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Finish threads
    threadExitFlag = true;
    //readingThread.join();
    
    // GLFW shutdown
    glfwTerminate();

    return 0;
}