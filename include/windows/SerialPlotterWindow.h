#ifndef _SERIALPLOTTERWINDOW__H_
#define _SERIALPLOTTERWINDOW__H_

#include <windows/Window.h>
#include <plotting/RealTimePlot.h>

namespace RTPlot
{
	// Interface
	class SerialPlotterWindow : public Window
	{
	private:
		RealTimePlot* plotter;

	public:
		SerialPlotterWindow(RealTimePlot* _plotter) : Window(WindowType::serialPlotter), plotter(_plotter) { }

		void render(void)
		{
			if (shouldRender)
			{
				ImGui::Begin("RTPlot - by AdriTeixeHax", &closable);
				if (plotter) plotter->plot();
				ImGui::End();
			}
		}
	};
}

#endif