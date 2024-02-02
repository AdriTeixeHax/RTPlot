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
	enum class WindowType { none, demo, serialPlotter };

	// Interface
	class Window
	{
	protected:
		WindowType type;
		bool shouldRender = true;
		bool closable = true;

	public:
		Window(void) : type(WindowType::none) { }
		Window(WindowType _type) : type(_type) { }

		WindowType getType(void) const { return type; }

		bool canRender(void) const { return shouldRender; }
		void enable(void) { shouldRender = true; }
		void disable(void) { shouldRender = false; }

		virtual void render(void) = 0;
	};
}

#endif