#include <RollingBuffer.h>

namespace RTPlot // Had to rename the namespace because of linker issues. Class taken and slightly modified from the implot_demo.cpp file.
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

    RollingBuffer& RollingBuffer::operator=(const RollingBuffer& rbuf)
    {
        this->span = 10.0f;
        this->plotFlag = rbuf.plotFlag;
        this->data.reserve(2000);
        for (size_t i = 0; i < fmin(data.size(), rbuf.data.size()); i++)
            this->data[i] = rbuf.data[i];
        return *this;
    }

    void RollingBuffer::AddPoint(double x, double y)
    {
        float xmod = fmod(x, span);
        if (!data.empty() && xmod < data.back().x)
            data.shrink(0);
        data.push_back(ImVec2(xmod, y));
    }

    JSON RollingBuffer::toJSON(void)
    {
        return
        {
            {"span", span},
            {"plotFlag", plotFlag}
        };
    }
    void RollingBuffer::fromJSON(const JSON& j)
    {
        j.at("span").get_to(span);
        j.at("plotFlag").get_to(plotFlag);
    }
}