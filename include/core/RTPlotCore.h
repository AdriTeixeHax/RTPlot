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
#include <stb_image/stb_image.h>

// Serial communication
#include <DeviceManager.h>

// Plotting
#include <Logger.h>

#include <RTPlotFunctions.h>

namespace RTPlot
{
    class RTPlotCore
    {
        // Flags
        bool verboseFlag        = true;
        bool ImGuiDemoFlag      = false;
        bool ImPlotDemoFlag     = false;
        bool consoleLogFlag     = true;
        bool showAddPlotFlag    = false;
        bool serialOptionsFlag  = true;
        bool showDeletePlotFlag = true;

        std::vector<uint8_t> serialPorts;
        std::string          logMsg;
		ImGui::Log		     log;

    public:
        GLFWwindow*   window;
        ImFont*       largeFont;
        DeviceManager deviceManager;

    public:
        RTPlotCore(void) : window(nullptr), largeFont(new ImFont) { }
        ~RTPlotCore(void) { } // window and font pointers are deleted by the ImGui library
        
        bool GraphicsInit       (void);
        bool GuiInit            (void);
        bool LoadLogo           (void);
        void NewFrame           (void);
        void EndFrame           (void);
        void MenuBar            (void);
        void SerialOptionsWindow(void);
        void WelcomeWindow      (void);
        void DemoWindows        (void);
        void ShowLog            (void);
        void DeleteComponents   (void);
        void ShutDown           (void);
        ImFont* GetLargeFontPtr (void);
    };
}

#endif