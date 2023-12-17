/* main.cpp | Main file for RTPlot by AdriTeixeHax */

// IO
#include <iostream>

// Callbacks
#include "Input.h"

// Graphics
#include "Shader.h"
#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Texture.h"

// OpenGL math
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// UI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

// Tests
#include "tests/TestClearColor.h"
#include "tests/TestTexture2D.h"

int main(int argc, char** argv)
{
    // Create window context
    Window* window;
    // Initialize GLFW
    if (!glfwInit()) { std::cerr << "[GLFW]: Error initializing GLFW." << std::endl; return -1; }
    // Set OpenGL version: 4.6 core
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(960, 540, "RTPlot - by AdriTeixeHax", NULL, NULL);
    if (!window) { std::cerr << "[GLFW]: Error creating window." << std::endl; glfwTerminate(); return -1; }

    glfwSetKeyCallback(window, OnKeyboard); // Input callback registering
    glfwMakeContextCurrent(window);         // Make the window's context current

    // Initialize GLEW
    if (glewInit() != GLEW_OK) { std::cerr << "Error initializing GLEW." << std::endl; return -1; }

    // Print out the OpenGL version
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl << std::endl;

    // Enable and set blending
    glCall(glEnable(GL_BLEND));
    glCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    // Create rendering instance
    Renderer renderer;

    /* ImGui context creation and configuration */
    ImGui::CreateContext();                                 // Create an ImGui context
    ImGuiIO& io = ImGui::GetIO(); (void)io;                 // Get the IO configuration from ImGui
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;   // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;    // Enable Gamepad Controls
    ImGui::StyleColorsDark();                               // Set the default color scheme to dark
    ImGui_ImplGlfw_InitForOpenGL(window, true);             // Link the GLFW context with ImGui
    ImGui_ImplOpenGL3_Init("#version 130");                 // Initialize the OpenGL version for ImGui

    //Model model(2U, 2U, 8U, 8U, 6U, positions, indices, glm::vec3(480.0f, 270.0f, 0.0f));
    //test::TestTextures test(proj, view, model, "res/shaders/Basic.shader", "res/textures/test.png", &renderer);

    // Tests
    test::Test* currentTest = nullptr;
    test::TestMenu* testMenu = new test::TestMenu(currentTest);
    currentTest = testMenu;

    testMenu->registerTest<test::TestClearColor>("Clear color");
    testMenu->registerTest<test::TestTexture2D>("2D Texture");

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

        if (currentTest)
        {
            currentTest->OnUpdate(0.0f);
            currentTest->OnRender();
            ImGui::Begin("Test");
            if (currentTest != testMenu && ImGui::Button("<-"))
            {
                delete currentTest;
                currentTest = testMenu;
            }
            currentTest->OnImGuiRender();
            ImGui::End();
        }

        // GUI rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll and process events
        glfwPollEvents();
    }

    delete currentTest;
    if (currentTest != testMenu) delete testMenu;

    // GUI shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();

    // GLFW shutdown
    glfwTerminate();

    return 0;
}