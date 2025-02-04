#ifndef _SCROLLINGBUFFER__H_
#define _SCROLLINGBUFFER__H_

#include <../Graphics.h>

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