#include "levelobjects.h"

namespace Render
{
    void LevelObjects::resize(int32_t x, int32_t y)
    {
        mData.resize(x*y);
        mWidth = x;
        mHeight = y;
    }

    std::vector<LevelObject>& get(int32_t x, int32_t y, LevelObjects& objs)
    {
        return objs.mData[x+y*objs.mWidth];
    }

    Misc::Helper2D<LevelObjects, std::vector<LevelObject>&> LevelObjects::operator[] (int32_t x)
    {
        return Misc::Helper2D<LevelObjects, std::vector<LevelObject>&>(*this, x, get);
    }

    int32_t LevelObjects::width()
    {
        return mWidth;
    }

    int32_t LevelObjects::height()
    {
        return mHeight;
    }
}
