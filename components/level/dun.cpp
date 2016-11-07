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
       
        mBlocks.resize(mWidth*mHeight);
        f.FAfread(&mBlocks[0], 2, mWidth*mHeight);

        std::cout << "w: " << mWidth << ", h: " << mHeight << std::endl;
    }

    Dun::Dun(size_t width, size_t height)
    {
        resize(width, height);
    }

    Dun::Dun() {}

    void Dun::resize(size_t width, size_t height)
    {
        mWidth = width;
        mHeight = height;
        mBlocks.resize(mWidth*mHeight, 0);    
    }

    Dun Dun::getTown(const Dun& sector1, const Dun& sector2, const Dun& sector3, const Dun& sector4)
    {
        Dun town(48, 48);
        
        for(size_t x = 0; x < sector3.mWidth; x++)
        {
            for(size_t y = 0; y < sector3.mHeight; y++)
            {
                town[0+x][23+y] = sector3[x][y];
            }
        }

        for(size_t x = 0; x < sector4.mWidth; x++)
        {
            for(size_t y = 0; y < sector4.mHeight; y++)
            {
                town[0+x][0+y] = sector4[x][y];
            }
        }

        for(size_t x = 0; x < sector1.mWidth; x++)
        {
            for(size_t y = 0; y < sector1.mHeight; y++)
            {
                town[23+x][23+y] = sector1[x][y];
            }
        }

        for(size_t x = 0; x < sector2.mWidth; x++)
        {
            for(size_t y = 0; y < sector2.mHeight; y++)
            {
                town[23+x][0+y] = sector2[x][y];
            }
        }

        return town;
    }

    int16_t& get(size_t x, size_t y, Dun& dun)
    {
        return dun.mBlocks[x+y*dun.width()];
    }

    const int16_t& get(size_t x, size_t y, const Dun& dun)
    {
        return dun.mBlocks[x+y*dun.width()];
    }

    Misc::Helper2D<Dun, int16_t&> Dun::operator[] (size_t x)
    {
        return Misc::Helper2D<Dun, int16_t&>(*this, x, get);
    }

    Misc::Helper2D<const Dun, const int16_t&> Dun::operator[] (size_t x) const
    {
        return Misc::Helper2D<const Dun, const int16_t&>(*this, x, get);
    }

    size_t Dun::width() const
    {
        return mWidth;
    }

    size_t Dun::height() const
    {
        return mHeight;
    }
}
