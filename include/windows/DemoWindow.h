#ifndef _DEMOWINDOW__H_
#define _DEMOWINDOW__H_

#include <windows/Window.h>

namespace RTPlot
{
	class DemoWindow : public Window
	{
	public:
		DemoWindow(void) : Window(WindowType::demo) { }
		void render(void)
		{
			if (shouldRender)
			{
				ImGui::Begin("RTPlot - by AdriTeixeHax");
				ImPlot::ShowDemoWindow();
				ImGui::End();
			}
		}
	};
}

#endif