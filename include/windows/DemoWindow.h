#ifndef _DEMOWINDOW__H_
#define _DEMOWINDOW__H_

#include <windows/Window.h>

namespace RTPlot
{
	// Interface
	class DemoWindow : public Window
	{
	public:
		void render(void)
		{
			ImGui::Begin("RTPlot - by AdriTeixeHax");
				ImPlot::ShowDemoWindow();
			ImGui::End();
		}
	};
}

#endif