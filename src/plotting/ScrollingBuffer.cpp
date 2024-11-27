#include <../include/plotting/ScrollingBuffer.h>

namespace RTPlot // Had to rename the namespace because of linker issues
{
    ScrollingBuffer::ScrollingBuffer(int max_size)
    {
        maxSize = max_size;
        offset = 0;
        data.reserve(maxSize);
    }

    void ScrollingBuffer::AddPoint(float x, float y)
    {
        if (data.size() < maxSize)
            data.push_back(ImVec2(x, y));
        else
        {
            data[offset] = ImVec2(x, y);
            offset = (offset + 1) % maxSize;
        }
    }

    void ScrollingBuffer::Erase(void)
    {
        if (data.size() > 0)
        {
            data.shrink(0);
            offset = 0;
        }
    }
}