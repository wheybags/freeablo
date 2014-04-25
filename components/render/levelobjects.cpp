#include "levelobjects.h"

namespace Render
{
    void LevelObjects::resize(size_t x, size_t y)
    {
        mData.resize(x*y);
        mWidth = x;
        mHeight = y;
    }

    LevelObject& get(size_t x, size_t y, LevelObjects& objs)
    {
        return objs.mData[x+y*objs.mWidth];
    }

    Misc::Helper2D<LevelObjects, LevelObject&> LevelObjects::operator[] (size_t x)
    {
        return Misc::Helper2D<LevelObjects, LevelObject&>(*this, x, get);
    }
    
    size_t LevelObjects::width()
    {
        return mWidth;
    }

    size_t LevelObjects::height()
    {
        return mHeight;
    }
}
