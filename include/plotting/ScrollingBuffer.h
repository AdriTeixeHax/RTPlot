/// ScrollingBuffer.h - Class to manage a buffer that scrolls continuously.

#ifndef _SCROLLINGBUFFER__H_
#define _SCROLLINGBUFFER__H_

#include <../src/vendor/imgui/imgui.h>

namespace RTPlot // Had to rename the namespace because of linker issues
{
    class ScrollingBuffer // Taken from implot_demo.cpp
    {
    public:
        int maxSize;
        int offset;
        ImVector<ImVec2> data;

        ScrollingBuffer(int max_size = 2000);

        void AddPoint(float x, float y);
        void Erase(void);
    };
}

#endif