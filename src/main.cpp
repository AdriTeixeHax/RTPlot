/// main.cpp - Main file for RTPlot by AdriTeixeHax

#include <RTPlotCore.h>

/*******************************************************/

// Main function
int main(int argc, char** argv)
{
    /********************** SETUP **********************/

    // Create core instance
    RTPlot::RTPlotCore core;

    // Initialize graphics, GUI and load the logo
    if (!core.GraphicsInit()) return -1;
    if (!core.GuiInit())      return -1;
    if (!core.LoadLogo())     return -1;

    /******************** MAIN LOOP ********************/

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(core.window))
    {
		// Start the frame
        core.NewFrame();

        // Main window
		core.WelcomeWindow(); 
        core.RenderObjects();

        // Menus and options
        core.MenuBar();
        core.DemoWindows();

        // Device managing
		core.DeleteComponents();
        
        // Show log and end frame
        core.ShowLog(); // Plot the log message of the current cycle
        core.EndFrame();
    } // !while

    core.deviceManager.~DeviceManager(); // To end threads present in its components.

    core.ShutDown();

    return 0;
}