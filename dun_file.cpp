#include <stdio.h>
#include <iostream>

#include "dun_file.h"

DunFile::DunFile(const std::string& filename)
{
    FILE* f = fopen(filename.c_str(), "rb");
    
    fread(&mWidth, 2, 1, f);
    fread(&mHeight, 2, 1, f);
   
    mBlocks.resize(mWidth*mHeight);
    fread(&mBlocks[0], 2, mWidth*mHeight, f);

    fclose(f);

    std::cout << "w: " << mWidth << ", h: " << mHeight << std::endl;
}

DunFile::DunFile(size_t width, size_t height)
{
    mWidth = width;
    mHeight = height;
    mBlocks.resize(mWidth*mHeight, 0);    
}

DunFile getTown(const DunFile& sector1, const DunFile& sector2, const DunFile& sector3, const DunFile& sector4)
{
    DunFile town(48, 48);
    
    for(size_t x = 0; x < sector3.mWidth; x++)
    {
        for(size_t y = 0; y < sector3.mHeight; y++)
        {
            town.at(0+x, 23+y) = sector3[x][y];
        }
    }

    for(size_t x = 0; x < sector4.mWidth; x++)
    {
        for(size_t y = 0; y < sector4.mHeight; y++)
        {
            town.at(0+x, 0+y) = sector4[x][y];
        }
    }

    for(size_t x = 0; x < sector1.mWidth; x++)
    {
        for(size_t y = 0; y < sector1.mHeight; y++)
        {
            town.at(23+x, 23+y) = sector1[x][y];
        }
    }

    for(size_t x = 0; x < sector2.mWidth; x++)
    {
        for(size_t y = 0; y < sector2.mHeight; y++)
        {
            town.at(23+x, 0+y) = sector2[x][y];
        }
    }









    return town;
}

