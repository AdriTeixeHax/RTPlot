#ifndef _WINDOW__H_
#define _WINDOW__H_

// UI
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
// Plotting
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	// Interface
	class Window
	{
		virtual void render(void) = 0;
	};
}

#endif