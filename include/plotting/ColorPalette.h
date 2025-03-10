/// ColorPalette.h - Header file for the ColorPalette class, responsible for managing the color of the plot and its color picker.

#ifndef _COLORPALETTE__H_
#define _COLORPALETTE__H_

#include <stdint.h>

#include <../src/vendor/imgui/imgui.h>

namespace RTPlot
{
	class ColorPalette
	{
		uint32_t id;
		ImVec4   color;

	public:
		ColorPalette(ImVec4 _color, uint32_t _id);
		ColorPalette& operator=(const ColorPalette& _colorPalette)
		{
			id    = _colorPalette.id;
			color = _colorPalette.color;
			return *this;
		}

		// Getters
		ImVec4   GetColor(void) { return color; }
		uint32_t GetID(void) { return id; }

		// Actions
		void     ColorPicker(void);
	};
}

#endif