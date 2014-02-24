#include "min.h"

#include <stdio.h>
#include <iostream>

#include <faio/faio.h>
#include <misc/stringops.h>

namespace Level
{
    MinPillar::MinPillar(std::vector<int16_t> data): mData(data) {}

    size_t MinPillar::size() const
    {
        return mData.size();
    }

    int16_t MinPillar::operator[] (size_t index) const
    {
        return mData[index];
    }

    bool MinPillar::passable() const
    {
        for(size_t i = 0; i < mData.size() - 2; i++)
            if(mData[i] & 0x0FFF)
                return false;
        
        return true;
    }
    
    Min::Min(const std::string& filename)
    {

        FAIO::FAFile* minF = FAIO::FAfopen(filename);

        size_t minSize;
        // These two files contain 16 blocks, all else are 10. Nothing to do but a workaround...
        if(Misc::StringUtils::endsWith(filename, "l4.min") || Misc::StringUtils::endsWith(filename, "town.min"))
            minSize = 16;
        else
            minSize = 10;

        size_t numPillars = FAIO::FAsize(minF)/minSize; 
        
        FAIO::FAfseek(minF, 0, SEEK_SET);
        
        std::vector<int16_t> temp(minSize);
        
        for(size_t i = 0; i < numPillars; i++)
        {
            FAIO::FAfread(&temp[0], 2, minSize, minF);
            mPillars.push_back(MinPillar(temp));
        }
        
        FAIO::FAfclose(minF);
    }
         
    const MinPillar& Min::operator[] (size_t index) const
    {
        return mPillars[index];
    }

    size_t Min::size()
    {
        return mPillars.size();
    }
}
