#ifndef _MENU__H_
#define _MENU__H_

#include <vector>

#include <windows/MainMenuWindow.h>

namespace RTPlot
{
	class Menu
	{
		RTPlot::MainMenuWindow menuWindow;
		std::vector<RTPlot::Window*> windows;

	public:
		bool addWindow(RTPlot::Window* w)
		{
			if (w) windows.push_back(w);
			else std::cerr << "[MENU]: Can't add window because it is a nullptr." << std::endl; return false; 
			return true;
		}

		bool render(void)
		{
			for (RTPlot::Window* window : windows)
			{
				if (!window) { std::cerr << "[MENU]: Can't render window because of nullptr dereference." << std::endl; return false; }

				window->render();
			}
			return true;
		}
	};
}

#endif