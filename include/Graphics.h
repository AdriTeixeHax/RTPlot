#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

// OpenGL libraries
#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Plotting and UI
#include <implot/implot.h>
#include <implot/implot_internal.h>
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>

#include <stb_image/stb_image.h>

#include <RTPlotVars.h>

namespace RTPlot
{
    class Graphics
    {
    public:
        GLFWwindow* window;
        ImFont* largeFont;

    public:
        Graphics(void) : window(nullptr), largeFont(nullptr) { }
        ~Graphics(void) { /* window pointer is deleted by the ImGui library */ }
        
        bool GraphicsInit(void);
        bool GuiInit(void);
        bool LoadLogo(void);
        void NewFrame(void);
        void GuiEnd(void);
        void EndFrame(void);

        ImFont* GetLargeFontPtr(void);
    };
}

#endif