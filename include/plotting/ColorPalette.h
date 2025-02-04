#ifndef _COLORPALETTE__H_
#define _COLORPALETTE__H_

#include <../Graphics.h>

namespace RTPlot
{
	class ColorPalette
	{
		ImVec4 color;
		uint32_t id;

	public:
		ColorPalette(ImVec4 _color, uint32_t _id);

		ImVec4 GetColor(void) { return color; }
		void ColorPicker(void);
	};
}

#endif