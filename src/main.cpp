/* main.cpp | Main file for RTPlot by AdriTeixeHax */

// IO
#include <iostream>

// Callbacks and threads
#include <thread>
//#include "Input.h"

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

// UI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Plotting
#include "plotting/RealTimePlot.h"

// Serial devices
#include "SerialDevice.h"

// Tests
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

// Window name
#define RTPLOT_WINDOW_NAME "RTPlot - by AdriTeixeHax"

// Font paths
#define RTPLOT_SOURCESANS_PATH "res/fonts/SourceSans/SourceSans3-Regular.ttf"
#define RTPLOT_SOURCESANS_SIZE 20
#define RTPLOT_CONSOLA_PATH "res/fonts/Consola/Consola.ttf"
#define RTPLOT_CONSOLA_SIZE 15

#define RTPLOT_WINDOW_RADIUS 5
#define RTPLOT_MAIN_RADIUS 3

// Logo paths
#define RTPLOT_LOGO_PATH "res/logos/RTPlot.png"

/**********************************************************************************************/

// Serial devices
RTPlot::SerialDevice arduino("COM3");

// Plotting tools
RTPlot::RealTimePlot plotter;

// Reading thread
void serialReadingFunc(bool* exitFlag)
{
    while (!*exitFlag)
    {
        arduino.recieve(RTPLOT_BYTE_SIZE);
    }
}

int main(int argc, char** argv)
{
    // Create window context and get data from main monitor
    GLFWwindow* window;

    // Initialize GLFW
    if (!glfwInit()) { std::cerr << "[GLFW]: Error initializing GLFW." << std::endl; return -1; }
    // Set OpenGL version: 4.6 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(1920, 1080, RTPLOT_WINDOW_NAME, /*glfwGetPrimaryMonitor()*/ NULL, NULL);
    if (!window) { std::cerr << "[GLFW]: Error creating window." << std::endl; glfwTerminate(); return -1; }

    // Initialize threads
    bool threadExitFlag = false;
    std::thread readingThread(serialReadingFunc, &threadExitFlag);

    // Register callbacks
    //glfwSetKeyCallback(window, OnKeyboard); // Input callback registering
    glfwMakeContextCurrent(window);         // Make the window's context current

    // Load window icon
    int width, height, channels;
    unsigned char* pixels = stbi_load(RTPLOT_LOGO_PATH, &width, &height, &channels, 4);

    // Change window icon
    GLFWimage images[1];
    images[0].width = width;
    images[0].height = height;
    images[0].pixels = pixels;
    glfwSetWindowIcon(window, 1, images);

    // Initialize GLEW
    if (glewInit() != GLEW_OK) { std::cerr << "Error initializing GLEW." << std::endl; return -1; }

    // Print out the OpenGL version
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl << std::endl;

    // Enable and set blending
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // Create a rendering instance
    Renderer renderer;

    // ImGui and ImPlot context creation
    ImGui::CreateContext();
    ImPlot::CreateContext();

    /** ImGui configuration **/
    // ImGui IO configuration
    ImGuiIO& io = ImGui::GetIO(); (void)io;                                       // Get the IO configuration from ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;                         // Enable keyboard controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;                          // Enable gamepad controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;                             // Enable docking
    //io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
    io.Fonts->AddFontFromFileTTF(RTPLOT_SOURCESANS_PATH, RTPLOT_SOURCESANS_SIZE); // Add the SourceSans font
    io.Fonts->AddFontFromFileTTF(RTPLOT_CONSOLA_PATH,    RTPLOT_CONSOLA_SIZE);    // Add the Consolas font
    // ImGui element radius configuration
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

        plotter.plot((void*)0);
        //ImPlot::ShowDemoWindow();

        // GUI rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll and process events
        glfwPollEvents();
    }

    // GUI shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // Thread finish
    threadExitFlag = true;
    readingThread.join();

    // GLFW shutdown
    glfwTerminate();

    return 0;
}