/// ColorPalette.h - Header file for the ColorPalette class, responsible for managing the color of the plot and its color picker.

#ifndef _COLORPALETTE__H_
#define _COLORPALETTE__H_

#include <stdint.h>

#include <imgui/imgui.h>

#include <nlohmann/json.hpp>
#include <imgui/imstb_rectpack.h>

using JSON = nlohmann::json;

namespace RTPlot
{
	struct RTVec4
	{
		float x = 0, y = 0, z = 0, w = 0;
		RTVec4(void) { }
		RTVec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
		RTVec4(const RTVec4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) { }
		RTVec4& operator= (const RTVec4& vec)
		{
			this->x = vec.x;
			this->y = vec.y;
			this->z = vec.z;
			this->w = vec.w;
			return *this;
		}
		static ImVec4 RTVec4ToImVec4(const RTVec4& vec) { return ImVec4(vec.x, vec.y, vec.z, vec.w); }
		static RTVec4 ImVec4ToRTVec4(const ImVec4& vec) { return RTVec4(vec.x, vec.y, vec.z, vec.w); }
	};

	inline JSON Vec4ToJSON(ImVec4 vec)
	{
		RTVec4 tempVec;
		tempVec.x = vec.x;
		tempVec.y = vec.y;
		tempVec.z = vec.z;
		tempVec.w = vec.w;
		return
		{
			{"x", tempVec.x},
			{"y", tempVec.y},
			{"z", tempVec.z},
			{"w", tempVec.w}
		};
	}

	inline void JSONToVec4(const JSON& j, ImVec4& vec)
	{
		RTVec4 tempVec;

		if (j.contains("x")) 
			j.at("x").get_to(tempVec.x);
		if (j.contains("y")) 
			j.at("y").get_to(tempVec.y);
		if (j.contains("z")) 
			j.at("z").get_to(tempVec.z);
		if (j.contains("w")) 
			j.at("w").get_to(tempVec.w);

		vec = RTVec4::RTVec4ToImVec4(tempVec);
	}

	class ColorPalette
	{
		uint32_t id;
		ImVec4   color;

	public:
		ColorPalette(void) : id(0), color(ImVec4(0,0,0,0)) { }
		ColorPalette(const ImVec4& _color, uint32_t _id);
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
		JSON     toJSON(void);
		void	 fromJSON(const JSON& j);
	};
}

#endif