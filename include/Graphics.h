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

#include <iostream>

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
        Graphics(void) : window(nullptr), largeFont(new ImFont) { }
        ~Graphics(void) { } // window and font pointers are deleted by the ImGui library
        
        bool GraphicsInit(void)
        {
            // Initialize GLFW
            if (!glfwInit()) { std::cerr << "[GLFW]: Error initializing GLFW." << std::endl; return false; }

            // Set OpenGL version: 4.6 core
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            // Create a windowed mode window and its OpenGL context
            window = glfwCreateWindow(1920, 1080, RTPLOT_WINDOW_NAME, NULL, NULL);
            if (!window) { std::cerr << "[GLFW]: Error creating window." << std::endl; glfwTerminate(); return false; }

            // Make the window's context current
            glfwMakeContextCurrent(window);

            // Initialize GLEW
            if (glewInit() != GLEW_OK) { std::cerr << "[GLEW]: Error initializing GLEW." << std::endl; return false; }

            // Print out the OpenGL version
            std::cout << "[OPENGL]: OpenGL version: " << glGetString(GL_VERSION) << std::endl << std::endl;

            // Enable and set blending
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            return true;
        }

        bool GuiInit(void)
        {
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
            io.Fonts->AddFontFromFileTTF(RTPLOT_CONSOLA_PATH, RTPLOT_CONSOLA_SIZE);       // Add the Consolas font
            largeFont = io.Fonts->AddFontFromFileTTF(RTPLOT_SOURCESANS_PATH, RTPLOT_SOURCESANS_SIZE_LARGE);

            // ImGui element radius setting
            ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, RTPLOT_MAIN_RADIUS);
            ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, RTPLOT_MAIN_RADIUS);
            ImGui::PushStyleVar(ImGuiStyleVar_GrabRounding, RTPLOT_MAIN_RADIUS);
            ImGui::PushStyleVar(ImGuiStyleVar_PopupRounding, RTPLOT_MAIN_RADIUS);
            ImGui::PushStyleVar(ImGuiStyleVar_ScrollbarRounding, RTPLOT_MAIN_RADIUS);
            ImGui::PushStyleVar(ImGuiStyleVar_TabRounding, RTPLOT_MAIN_RADIUS);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, RTPLOT_WINDOW_RADIUS);

            // Set the default color scheme to dark
            ImGui::StyleColorsDark();

            // Link the GLFW context with ImGui and initialize the OpenGL version for ImGui
            ImGui_ImplGlfw_InitForOpenGL(window, true);
            ImGui_ImplOpenGL3_Init("#version 130");

            // Disable ugly menu arrow
            ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;

            return true;
        }

        bool LoadLogo(void)
        {
            // Load app logo
            GLFWimage* appLogo = new GLFWimage();
            appLogo->pixels = stbi_load(RTPLOT_LOGO_PATH, &appLogo->width, &appLogo->height, nullptr, 4);
            glfwSetWindowIcon(window, 1, appLogo);
            delete appLogo;

            return true;
        }
        
        void NewFrame(void)
        {
            // Clear color buffer bit
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            // renderer.clear();

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
        }

        void GuiEnd(void)
        {
            // GUI rendering
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }

        void EndFrame(void)
        {
            // Update and Render additional Platform Windows
            ImGuiIO& io = ImGui::GetIO();
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                GLFWwindow* backup_current_context = glfwGetCurrentContext();
                ImGui::UpdatePlatformWindows();
                ImGui::RenderPlatformWindowsDefault();
                glfwMakeContextCurrent(backup_current_context);
            }

            glfwSwapBuffers(window); // Swap front and back buffers
            glfwPollEvents(); // Poll and process events
        }

        ImFont* GetLargeFontPtr(void)
        {
            if (largeFont) return largeFont;
            else return nullptr;
        }
    };
}

#endif