#include <stdio.h>
#include <iostream>

#include "dun.h"

#include <faio/fafileobject.h>

namespace Level
{
    Dun::Dun(const std::string& filename)
    {
        FAIO::FAFileObject f(filename);

        int16_t temp;
        f.FAfread(&temp, 2, 1);
        mWidth = temp;
        f.FAfread(&temp, 2, 1);
        mHeight = temp;

        std::vector<int16_t> buf (mWidth*mHeight);
        f.FAfread(buf.data (), 2, mWidth*mHeight);
        mBlocks.resize(buf.size ());
        std::copy (buf.begin (), buf.end (), mBlocks.begin ());

        std::cout << "w: " << mWidth << ", h: " << mHeight << std::endl;
    }

    Dun::Dun(int32_t width, int32_t height)
    {
        resize(width, height);
    }

    Dun::Dun() {}

    void Dun::resize(int32_t width, int32_t height)
    {
        mWidth = width;
        mHeight = height;
        mBlocks.resize(mWidth*mHeight, 0);
    }

    Dun Dun::getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4)
    {
        Dun town(48, 48);

        for(int32_t x = 0; x < sector3.mWidth; x++)
        {
            for(int32_t y = 0; y < sector3.mHeight; y++)
            {
                town[0+x][23+y] = sector3[x][y];
            }
        }

        for(int32_t x = 0; x < sector4.mWidth; x++)
        {
            for(int32_t y = 0; y < sector4.mHeight; y++)
            {
                town[0+x][0+y] = sector4[x][y];
            }
        }

        for(int32_t x = 0; x < sector1.mWidth; x++)
        {
            for(int32_t y = 0; y < sector1.mHeight; y++)
            {
                town[23+x][23+y] = sector1[x][y];
            }
        }

        for(int32_t x = 0; x < sector2.mWidth; x++)
        {
            for(int32_t y = 0; y < sector2.mHeight; y++)
            {
                town[23+x][0+y] = sector2[x][y];
            }
        }

        return town;
    }

    int32_t& get(int32_t x, int32_t y, Dun& dun)
    {
        return dun.mBlocks[x+y*dun.width()];
    }

    const int32_t& get(int32_t x, int32_t y, const Dun& dun)
    {
        return dun.mBlocks[x+y*dun.width()];
    }

    Misc::Helper2D<Dun, int32_t&> Dun::operator[] (int32_t x)
    {
        return Misc::Helper2D<Dun, int32_t&>(*this, x, get);
    }

    Misc::Helper2D<const Dun, const int32_t&> Dun::operator[] (int32_t x) const
    {
        return Misc::Helper2D<const Dun, const int32_t&>(*this, x, get);
    }

    int32_t Dun::width() const
    {
        return mWidth;
    }

    int32_t Dun::height() const
    {
        return mHeight;
    }
}
