#include <../include/plotting/RollingBuffer.h>

namespace RTPlot // Had to rename the namespace because of linker issues. Class taken and slightly modified from the implot_demo.cpp file.
{
    RollingBuffer::RollingBuffer(const std::string& _name) : name(_name), tempName(" ")
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
        float xmod = fmodf(x, span);
        if (!data.empty() && xmod < data.back().x)
            data.shrink(0);
        data.push_back(ImVec2(xmod, y));
    }
}