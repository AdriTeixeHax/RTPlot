#include <RTPlotCore.h>

bool RTPlot::RTPlotCore::GraphicsInit(void)
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

bool RTPlot::RTPlotCore::GuiInit(void)
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

bool RTPlot::RTPlotCore::LoadLogo(void)
{
    // Load app logo
    GLFWimage* appLogo = new GLFWimage();
    appLogo->pixels = stbi_load(RTPLOT_LOGO_PATH, &appLogo->width, &appLogo->height, nullptr, 4);
    glfwSetWindowIcon(window, 1, appLogo);
    delete appLogo;

    return true;
}

void RTPlot::RTPlotCore::NewFrame(void)
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

    // Begin main window
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

void RTPlot::RTPlotCore::EndFrame(void)
{
    // End main window
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

    glfwSwapBuffers(window); // Swap front and back buffers
    glfwPollEvents(); // Poll and process events
}

ImFont* RTPlot::RTPlotCore::GetLargeFontPtr(void)
{
    if (largeFont) return largeFont;
    else return nullptr;
}

void RTPlot::RTPlotCore::MenuBar(void)
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Options"))
        {
            if (ImGui::MenuItem("ImPlot Demo", "", ImPlotDemoFlag)) { ImPlotDemoFlag = !ImPlotDemoFlag; }
            if (ImGui::MenuItem("ImGui Demo", "", ImGuiDemoFlag)) { ImGuiDemoFlag = !ImGuiDemoFlag; }
            ImGui::Separator();
            if (ImGui::MenuItem("Verbose data", "", verboseFlag))
            {
                verboseFlag = !verboseFlag;
                if (verboseFlag) logMsg = "Turned on verbose.\n";
                else             logMsg = "Turned off verbose.\n";
            }
            if (ImGui::MenuItem("Console log", "", consoleLogFlag)) { consoleLogFlag = !consoleLogFlag; }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Add device"))
        {
            serialPorts = RTPlot::SerialPort::ScanAvailablePorts();
            for (uint8_t device : serialPorts)
            {
                std::string tempName = "COM" + std::to_string(device);
                if (ImGui::MenuItem(tempName.c_str(), "", false))
                {
                    std::string portName = "\\\\.\\COM" + std::to_string(device);
                    deviceManager.AddDevice(portName.c_str(), logMsg);
                    logMsg = "Added device " + tempName + "\n";
                    showAddPlotFlag = false;
                }
            }
            if (serialPorts.size() == 0) ImGui::MenuItem("No free serial ports found", "", false);
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void RTPlot::RTPlotCore::WelcomeWindow(void)
{
    if (deviceManager.Size() == 0)
    {
        ImGui::Begin("RTPlot - by AdriTeixeHax", NULL); // Null so that it cannot be closed
            ImVec2 avail = ImGui::GetContentRegionAvail();

            static const char* welcome = "Welcome to RTPlot!";
            static const char* connectBtn = "Connect to a device...";
            ImVec2 welcomeSize = ImGui::CalcTextSize(welcome);

            static ImVec2 connectSize = ImVec2(0, 0);

            ImGui::SetCursorPosX((avail.x - welcomeSize.x) * 0.5f);
            ImGui::SetCursorPosY((avail.y - welcomeSize.y) * 0.5f - 15);
            ImGui::Text(welcome);

            ImGui::SetCursorPosX((avail.x - connectSize.x) * 0.5f);
            ImGui::SetCursorPosY((avail.y - connectSize.y) * 0.5f + 15);
            if (ImGui::Button(connectBtn))
            {
                serialPorts = RTPlot::SerialPort::ScanAvailablePorts();
                showAddPlotFlag = true;
            }
            connectSize = ImGui::GetItemRectSize();

            if (showAddPlotFlag)
            {
                static ImVec2 groupSize = ImVec2(0, 0);

                ImGui::SetCursorPosX((avail.x - groupSize.x) / 2);
                ImGui::SetCursorPosY((avail.y - groupSize.y) / 2 + 50);
                ImGui::BeginGroup();
                    for (uint8_t device : serialPorts)
                    {
                        std::string portName = "\\\\.\\COM" + std::to_string(device);
                        std::string portNameGUI = "COM" + std::to_string(device);

                        ImGui::PushID(device);
                            ImGui::PushStyleColor(ImGuiCol_Button,        (ImVec4)ImColor::HSV(device / 10.0f, 1.0f, 0.6f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(device / 10.0f, 0.7f, 0.7f));
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive,  (ImVec4)ImColor::HSV(device / 10.0f, 0.7f, 0.5f));
                                if (ImGui::Button(portNameGUI.c_str()))
                                {
                                    deviceManager.AddDevice(portName.c_str(), logMsg);
                                    showAddPlotFlag = false;
                                    logMsg = "Added device " + portNameGUI + "\n";
                                }
                            ImGui::PopStyleColor(3);
                        ImGui::PopID();

                        ImGui::SameLine();
                    }
                ImGui::EndGroup();

                groupSize = ImGui::GetItemRectSize();
                float posX = ImGui::GetCursorPosX();
                float posY = ImGui::GetCursorPosY();
            }
        ImGui::End();
    }
}

void RTPlot::RTPlotCore::DemoWindows(void)
{
    if (ImPlotDemoFlag) ImPlot::ShowDemoWindow(&ImPlotDemoFlag);
    if (ImGuiDemoFlag)   ImGui::ShowDemoWindow(&ImGuiDemoFlag);
}

void RTPlot::RTPlotCore::ShowLog(void)
{
    if (consoleLogFlag) log.ShowConsoleLog(logMsg, &consoleLogFlag);
}

void RTPlot::RTPlotCore::DeleteComponents(void)
{
    uint8_t counter = 0;
    for (auto component : deviceManager.GetComponents())
    {
        if (!component->GetKillFlag())
        {
            logMsg = "Deleted device " + RTPlot::StripPortNamePrefix(component->GetPortName()) + "\n";
            deviceManager.RemoveDevice(counter);
            counter = 0;
            break;
        }
        counter++;
    }
}

void RTPlot::RTPlotCore::ShutDown(void)
{
    // GUI shutdown
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    // GLFW shutdown
    glfwTerminate();
}