/// RTPlotCore.h - Class for handling main graphics calls and basic device management in the RTPlot project.

#ifndef __RTPLOTCORE_H__
#define __RTPLOTCORE_H__

// OpenGL libraries
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <implot/implot.h>

// Image loading
#include <stb/stb_image.h>

// Plotting
#include <Logger.h>

#include <RTPlotFunctions.h>

#include <FileManager.h>
#include <DeviceManager.h>

namespace RTPlot
{
    class RTPlotCore
    {
        // Flags
        bool                 verboseFlag        = true;
        bool                 ImGuiDemoFlag      = false;
        bool                 ImPlotDemoFlag     = false;
        bool                 consoleLogFlag     = true;
        bool                 showAddPlotFlag    = false;
        bool                 showDeletePlotFlag = true;
        bool                 loadFileFlag       = false;

        std::vector<uint8_t> serialPorts;
        std::string          logMsg;
		ImGui::Log		     log;

    public:
        GLFWwindow*          window;
        DeviceManager        deviceManager;
        FileManager          fileManager;

    public:
        RTPlotCore(void) : window(nullptr) { }
        ~RTPlotCore(void) { } // window and font pointers are deleted by the ImGui library
        
        bool GraphicsInit    (void);
        bool GuiInit         (void);
        bool LoadLogo        (void);
        void NewFrame        (void);
        void EndFrame        (void);
        void MenuBar         (void);
        void WelcomeWindow   (void);
        void RenderObjects   (void);
        void DemoWindows     (void);
        void ShowLog         (void);
        void DeleteComponents(void);
        void ShutDown        (void);
    };
}

#endif