#include <../include/plotting/RollingBuffer.h>

namespace RTPlot // Had to rename the namespace because of linker issues
{
    RollingBuffer::RollingBuffer(void)
    {
        span = 10.0f;
        data.reserve(2000);
    }

    RollingBuffer::~RollingBuffer(void)
    {
        data.clear();
    }

    void RollingBuffer::AddPoint(double x, double y)
    {
        double xmod = fmod(x, span);
        if (!data.empty() && xmod < data.back().x)
            data.shrink(0);
        data.push_back(ImVec2(xmod, y));
    }
}