#ifndef _MAINWINDOW__H_
#define _MAINWINDOW__H_

#include <windows/Window.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	// Interface
	class MainWindow : public Window
	{
	private:
		RealTimePlot* plotter;
	public:
		bool showDemoWindow = false;

	public:
		MainWindow(RealTimePlot* _plotter) : plotter(_plotter) { }

		void render(void)
		{
			ImGui::Begin("RTPlot - by AdriTeixeHax");
			ImGui::Checkbox("Demo Window", &showDemoWindow);
			if (plotter) plotter->plot();
			ImGui::End();
		}
	};
}

#endif