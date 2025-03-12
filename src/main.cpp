/// main.cpp - Main file for RTPlot by AdriTeixeHax

//#define FILEMANAGER_UNITTEST
#ifndef FILEMANAGER_UNITTEST

#include <RTPlotCore.h>

/*******************************************************/

 //Main function
int main(int argc, char** argv)
{
    /********************** SETUP **********************/

    // Create core instance
    RTPlot::RTPlotCore core;

    // Initialize graphics, GUI and load the logo
    if (!core.GraphicsInit()) return -1;
    if (!core.GuiInit())      return -1;
    if (!core.LoadLogo())     return -1;

    //// Create a rendering instance for custom models
    //RTPlot::Renderer renderer;

    /******************** MAIN LOOP ********************/

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(core.window))
    {
		// Start the frame
        core.NewFrame();

        // Main window
		core.WelcomeWindow(); 
        core.deviceManager.PlotAllDevices(); /// <---

        // Menus and options
        core.MenuBar();
        core.DemoWindows();

        // Device managing
		core.DeleteComponents(); /// <--- // Check if a device wishes to be deleted 
        
        // Show log and end frame
        core.ShowLog(); // Plot the log message of the current cycle
        core.EndFrame();
    } // !while

    core.deviceManager.~DeviceManager(); // To end threads present in its components.

    core.ShutDown();

    return 0;
}

#endif